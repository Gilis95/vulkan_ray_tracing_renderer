#include "include/assets/gltf/gltf_mesh_serializer.h"

#include "assets/components/mesh_component.h"
#include "glm/vec4.hpp"
#include "tinygltf/tinygltf_utils.h"
#include <tiny_gltf.h>

namespace wunder::gltf_mesh_serializer {
static std::optional<mesh_component> process_mesh(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, const std::string& mesh_name);

static bool try_parse_indices(const tinygltf::Model& gltf_scene_root,
                              const tinygltf::Primitive& gltf_primitive,
                              mesh_component& mesh_component);

static bool try_parse_positions(const tinygltf::Model& gltf_scene_root,
                                const tinygltf::Primitive& gltf_primitive,
                                mesh_component& mesh_component);
static void try_parse_mesh_tangents(const tinygltf::Model& gltf_scene_root,
                                    const tinygltf::Primitive& gltf_primitive,
                                    mesh_component& mesh_component);
static void try_parse_mesh_colour(const tinygltf::Model& gltf_scene_root,
                                  const tinygltf::Primitive& gltf_primitive,
                                  mesh_component& mesh_component);
static void try_parse_uvs(const tinygltf::Model& gltf_scene_root,
                          const tinygltf::Primitive& gltf_primitive,
                          mesh_component& out_mesh_component);
static void try_parse_normals(const tinygltf::Model& gltf_scene_root,
                              const tinygltf::Primitive& gltf_primitive,
                              mesh_component& out_mesh_component);
static void try_parse_aabb(const tinygltf::Model& gltf_scene_root,
                           const tinygltf::Primitive& gltf_primitive,
                           mesh_component& mesh_component);

std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<mesh_component>>
process_meshes(tinygltf::Model& gltf_scene_root,
               const std::unordered_set<uint32_t>& mesh_indices) {
  // Convert all mesh/primitives+ to a single primitive per mesh
  std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<mesh_component>>
      mesh_id_to_primitives;
  for (const auto& mesh_index : mesh_indices) {
    auto& gltf_mesh = gltf_scene_root.meshes[mesh_index];
    std::vector<mesh_component> mesh_nodes;
    for (const auto& gltf_primitive : gltf_mesh.primitives) {
      auto maybe_mesh_node =
          process_mesh(gltf_scene_root, gltf_primitive, gltf_mesh.name);
      ContinueUnless(maybe_mesh_node.has_value());
      mesh_nodes.push_back(std::move(maybe_mesh_node.value()));
    }

    mesh_id_to_primitives[mesh_index] = std::move(mesh_nodes);
  }

  return mesh_id_to_primitives;
}

std::optional<mesh_component> process_mesh(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, const std::string& mesh_name) {
  // Only triangles are supported
  // 0:point, 1:lines, 2:line_loop, 3:line_strip, 4:triangles, 5:triangle_strip,
  // 6:triangle_fan
  ReturnUnless(gltf_primitive.mode == 4, std::nullopt);

  mesh_component mesh_component;
  mesh_component.m_material_index = std::max(0, gltf_primitive.material);

  AssertReturnUnless(
      try_parse_indices(gltf_scene_root, gltf_primitive, mesh_component),
      std::nullopt);

  AssertReturnUnless(
      try_parse_positions(gltf_scene_root, gltf_primitive, mesh_component),
      std::nullopt);
  try_parse_aabb(gltf_scene_root, gltf_primitive, mesh_component);
  try_parse_normals(gltf_scene_root, gltf_primitive, mesh_component);
  try_parse_uvs(gltf_scene_root, gltf_primitive, mesh_component);
  try_parse_mesh_tangents(gltf_scene_root, gltf_primitive, mesh_component);
  try_parse_mesh_colour(gltf_scene_root, gltf_primitive, mesh_component);

  return mesh_component;
}

bool try_parse_indices(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Primitive& gltf_primitive,
                       mesh_component& mesh_component) {
  if (gltf_primitive.indices > -1) {
    const tinygltf::Accessor& indexAccessor =
        gltf_scene_root.accessors[gltf_primitive.indices];
    mesh_component.m_indecies.resize(
        static_cast<uint32_t>(indexAccessor.count));

    switch (indexAccessor.componentType) {
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
        tinygltf::utils::copy_accessor_data(mesh_component.m_indecies, 0,
                                            gltf_scene_root, indexAccessor, 0,
                                            indexAccessor.count);

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
    mesh_component.m_indecies.push_back(i);
  }

  return true;
}

bool try_parse_positions(const tinygltf::Model& gltf_scene_root,
                         const tinygltf::Primitive& gltf_primitive,
                         mesh_component& mesh_component) {
  std::vector<glm::vec3> positions;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec3>(
                   gltf_scene_root, gltf_primitive, positions, "POSITION"),
               false);

  mesh_component.m_verticies.resize(positions.size());
  for (std::uint32_t i = 0; i < positions.size(); ++i) {
    auto& position = positions[i];
    mesh_component.m_verticies[i].m_position = position;
  }

  return true;
}

void try_parse_aabb(const tinygltf::Model& gltf_scene_root,
                    const tinygltf::Primitive& gltf_primitive,
                    mesh_component& mesh_component) {
  const auto& accessor =
      gltf_scene_root
          .accessors[gltf_primitive.attributes.find("POSITION")->second];

  if (!accessor.minValues.empty()) {
    mesh_component.m_bounding_box.min = glm::vec3(
        accessor.minValues[0], accessor.minValues[1], accessor.minValues[2]);
  } else {
    mesh_component.m_bounding_box.min =
        glm::vec3(std::numeric_limits<float>::max());
    for (const auto& vertex : mesh_component.m_verticies) {
      for (int i = 0; i < 3; i++) {
        if (vertex.m_position[i] < mesh_component.m_bounding_box.min[i]) {
          mesh_component.m_bounding_box.min[i] = vertex.m_position[i];
        }
      }
    }
  }

  if (!accessor.maxValues.empty()) {
    mesh_component.m_bounding_box.max = glm::vec3(
        accessor.maxValues[0], accessor.maxValues[1], accessor.maxValues[2]);
  } else {
    mesh_component.m_bounding_box.max =
        glm::vec3(-std::numeric_limits<float>::max());
    for (const auto& vertex : mesh_component.m_verticies) {
      for (int i = 0; i < 3; i++) {
        if (vertex.m_position[i] > mesh_component.m_bounding_box.max[i]) {
          mesh_component.m_bounding_box.max[i] = vertex.m_position[i];
        }
      }
    }
  }
}

void try_parse_normals(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Primitive& gltf_primitive,
                       mesh_component& out_mesh_component) {
  std::vector<glm::vec3> normals;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec3>(
      gltf_scene_root, gltf_primitive, normals, "NORMAL"));

  for (uint32_t i = 0; i < normals.size(); ++i) {
    auto& normal = normals[i];
    out_mesh_component.m_verticies[i].m_normal = normal;
  }
}

void try_parse_uvs(const tinygltf::Model& gltf_scene_root,
                   const tinygltf::Primitive& gltf_primitive,
                   mesh_component& out_mesh_component) {
  std::vector<glm::vec2> tex_coords;
  bool texcoord_created = tinygltf::utils::get_attribute<glm::vec2>(
      gltf_scene_root, gltf_primitive, tex_coords, "TEXCOORD_0");
  if (!texcoord_created) {
    ReturnUnless(tinygltf::utils::get_attribute<glm::vec2>(
        gltf_scene_root, gltf_primitive, tex_coords, "TEXCOORD"));
  }

  for (uint32_t i = 0; i < tex_coords.size(); ++i) {
    auto& tex_coord = tex_coords[i];
    out_mesh_component.m_verticies[i].m_texcoord = tex_coord;
  }
}

void try_parse_mesh_tangents(const tinygltf::Model& gltf_scene_root,
                             const tinygltf::Primitive& gltf_primitive,
                             mesh_component& mesh_component) {
  std::vector<glm::vec4> tangents;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec4>(
      gltf_scene_root, gltf_primitive, tangents, "TANGENT"));

  for (uint32_t i = 0; i < tangents.size(); ++i) {
    auto& tangent = tangents[i];
    mesh_component.m_verticies[i].m_tangent = tangent;
  }
}

void try_parse_mesh_colour(const tinygltf::Model& gltf_scene_root,
                           const tinygltf::Primitive& gltf_primitive,
                           mesh_component& mesh_component) {  // COLOR_0
  std::vector<glm::vec4> colors;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec4>(
      gltf_scene_root, gltf_primitive, colors, "COLOR_0"));

  for (uint32_t i = 0; i < colors.size(); ++i) {
    auto& color = colors[i];
    mesh_component.m_verticies[i].m_color = color;
  }
}
}  // namespace wunder::gltf_mesh_serializer
