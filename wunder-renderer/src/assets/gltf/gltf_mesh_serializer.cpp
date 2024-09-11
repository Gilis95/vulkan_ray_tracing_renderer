#include "include/assets/gltf/gltf_mesh_serializer.h"

#include <tiny_gltf.h>

#include "assets/asset_storage.h"
#include "assets/asset_types.h"
#include "assets/components/mesh_asset.h"
#include "glm/vec4.hpp"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {

static bool try_parse_indices(const tinygltf::Model& gltf_scene_root,
                              const tinygltf::Primitive& gltf_primitive,
                              mesh_asset& mesh_asset);

static bool try_parse_positions(const tinygltf::Model& gltf_scene_root,
                                const tinygltf::Primitive& gltf_primitive,
                                mesh_asset& mesh_asset);
static void try_parse_mesh_tangents(const tinygltf::Model& gltf_scene_root,
                                    const tinygltf::Primitive& gltf_primitive,
                                    mesh_asset& mesh_asset);
static void try_parse_mesh_colour(const tinygltf::Model& gltf_scene_root,
                                  const tinygltf::Primitive& gltf_primitive,
                                  mesh_asset& mesh_asset);
static void try_parse_uvs(const tinygltf::Model& gltf_scene_root,
                          const tinygltf::Primitive& gltf_primitive,
                          mesh_asset& out_mesh_asset);
static void try_parse_normals(const tinygltf::Model& gltf_scene_root,
                              const tinygltf::Primitive& gltf_primitive,
                              mesh_asset& out_mesh_asset);
static void try_parse_aabb(const tinygltf::Model& gltf_scene_root,
                           const tinygltf::Primitive& gltf_primitive,
                           mesh_asset& mesh_asset);

std::optional<mesh_asset> gltf_mesh_serializer::process_mesh(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, const std::string& mesh_name,
    const std::unordered_map<uint32_t, asset_handle>& material_map) {
  // Only triangles are supported
  // 0:point, 1:lines, 2:line_loop, 3:line_strip, 4:triangles, 5:triangle_strip,
  // 6:triangle_fan
  ReturnUnless(gltf_primitive.mode == 4, std::nullopt);

  mesh_asset mesh_asset;
  auto found_material_it = material_map.find(gltf_primitive.material);
  mesh_asset.m_material_handle =
      found_material_it == material_map.end() ? 0 : found_material_it->second;

  AssertReturnUnless(
      try_parse_indices(gltf_scene_root, gltf_primitive, mesh_asset),
      std::nullopt);

  AssertReturnUnless(
      try_parse_positions(gltf_scene_root, gltf_primitive, mesh_asset),
      std::nullopt);
  try_parse_aabb(gltf_scene_root, gltf_primitive, mesh_asset);
  try_parse_normals(gltf_scene_root, gltf_primitive, mesh_asset);
  try_parse_uvs(gltf_scene_root, gltf_primitive, mesh_asset);
  try_parse_mesh_tangents(gltf_scene_root, gltf_primitive, mesh_asset);
  try_parse_mesh_colour(gltf_scene_root, gltf_primitive, mesh_asset);

  return mesh_asset;
}

bool try_parse_indices(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Primitive& gltf_primitive,
                       mesh_asset& mesh_asset) {
  if (gltf_primitive.indices > -1) {
    const tinygltf::Accessor& indexAccessor =
        gltf_scene_root.accessors[gltf_primitive.indices];
    mesh_asset.m_indecies.reserve(static_cast<uint32_t>(indexAccessor.count));

    auto lambda = [&gltf_scene_root, &indexAccessor,
                   &mesh_asset]<typename T>() {
      std::vector<T> indices(indexAccessor.count);
      tinygltf::utils::copy_accessor_data(
          indices, 0, gltf_scene_root, indexAccessor, 0, indexAccessor.count);

      std::move(indices.begin(), indices.end(),
                std::back_inserter(mesh_asset.m_indecies));
    };

    switch (indexAccessor.componentType) {
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
        lambda.operator()<std::int32_t>();
      } break;
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
        lambda.operator()<std::int16_t>();
      } break;
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
        lambda.operator()<std::int16_t>();
      } break;
      default:
        WUNDER_ERROR_TAG("Asset", "Index component type %i not supported!\n",
                         indexAccessor.componentType);
        return false;
    }

    return true;
  }

  // Primitive without indices, creating them
  const auto& accessor =
      gltf_scene_root
          .accessors[gltf_primitive.attributes.find("POSITION")->second];
  for (auto i = 0; i < accessor.count; i++) {
    mesh_asset.m_indecies.push_back(i);
  }

  return true;
}

bool try_parse_positions(const tinygltf::Model& gltf_scene_root,
                         const tinygltf::Primitive& gltf_primitive,
                         mesh_asset& mesh_asset) {
  std::vector<glm::vec3> positions;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec3>(
                   gltf_scene_root, gltf_primitive, positions, "POSITION"),
               false);

  mesh_asset.m_verticies.resize(positions.size());
  for (std::uint32_t i = 0; i < positions.size(); ++i) {
    auto& position = positions[i];
    mesh_asset.m_verticies[i].m_position = position;
  }

  return true;
}

void try_parse_aabb(const tinygltf::Model& gltf_scene_root,
                    const tinygltf::Primitive& gltf_primitive,
                    mesh_asset& mesh_asset) {
  const auto& accessor =
      gltf_scene_root
          .accessors[gltf_primitive.attributes.find("POSITION")->second];

  if (!accessor.minValues.empty()) {
    mesh_asset.m_bounding_box.min = glm::vec3(
        accessor.minValues[0], accessor.minValues[1], accessor.minValues[2]);
  } else {
    mesh_asset.m_bounding_box.min =
        glm::vec3(std::numeric_limits<float>::max());
    for (const auto& vertex : mesh_asset.m_verticies) {
      for (int i = 0; i < 3; i++) {
        if (vertex.m_position[i] < mesh_asset.m_bounding_box.min[i]) {
          mesh_asset.m_bounding_box.min[i] = vertex.m_position[i];
        }
      }
    }
  }

  if (!accessor.maxValues.empty()) {
    mesh_asset.m_bounding_box.max = glm::vec3(
        accessor.maxValues[0], accessor.maxValues[1], accessor.maxValues[2]);
  } else {
    mesh_asset.m_bounding_box.max =
        glm::vec3(-std::numeric_limits<float>::max());
    for (const auto& vertex : mesh_asset.m_verticies) {
      for (int i = 0; i < 3; i++) {
        if (vertex.m_position[i] > mesh_asset.m_bounding_box.max[i]) {
          mesh_asset.m_bounding_box.max[i] = vertex.m_position[i];
        }
      }
    }
  }
}

void try_parse_normals(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Primitive& gltf_primitive,
                       mesh_asset& out_mesh_asset) {
  std::vector<glm::vec3> normals;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec3>(
      gltf_scene_root, gltf_primitive, normals, "NORMAL"));

  for (uint32_t i = 0; i < normals.size(); ++i) {
    auto& normal = normals[i];
    out_mesh_asset.m_verticies[i].m_normal = normal;
  }
}

void try_parse_uvs(const tinygltf::Model& gltf_scene_root,
                   const tinygltf::Primitive& gltf_primitive,
                   mesh_asset& out_mesh_asset) {
  std::vector<glm::vec2> tex_coords;
  bool texcoord_created = tinygltf::utils::get_attribute<glm::vec2>(
      gltf_scene_root, gltf_primitive, tex_coords, "TEXCOORD_0");
  if (!texcoord_created) {
    ReturnUnless(tinygltf::utils::get_attribute<glm::vec2>(
        gltf_scene_root, gltf_primitive, tex_coords, "TEXCOORD"));
  }

  for (uint32_t i = 0; i < tex_coords.size(); ++i) {
    auto& tex_coord = tex_coords[i];
    out_mesh_asset.m_verticies[i].m_texcoord = tex_coord;
  }
}

void try_parse_mesh_tangents(const tinygltf::Model& gltf_scene_root,
                             const tinygltf::Primitive& gltf_primitive,
                             mesh_asset& mesh_asset) {
  std::vector<glm::vec4> tangents;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec4>(
      gltf_scene_root, gltf_primitive, tangents, "TANGENT"));

  for (uint32_t i = 0; i < tangents.size(); ++i) {
    auto& tangent = tangents[i];
    mesh_asset.m_verticies[i].m_tangent = tangent;
  }
}

void try_parse_mesh_colour(const tinygltf::Model& gltf_scene_root,
                           const tinygltf::Primitive& gltf_primitive,
                           mesh_asset& mesh_asset) {  // COLOR_0
  std::vector<glm::vec4> colors;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec4>(
      gltf_scene_root, gltf_primitive, colors, "COLOR_0"));

  for (uint32_t i = 0; i < colors.size(); ++i) {
    auto& color = colors[i];
    mesh_asset.m_verticies[i].m_color = color;
  }
}
}  // namespace wunder
