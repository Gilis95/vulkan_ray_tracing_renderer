#include "include/assets/serializers/gltf/gltf_mesh_serializer.h"

#include "assets/asset_storage.h"
#include "assets/asset_types.h"
#include "assets/mesh_asset.h"
#include "glm/vec4.hpp"
#include "tiny_gltf.h"
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
static void create_tangents(const mesh_asset& mesh_asset,
                            std::vector<glm::vec4>& out_tangents);

// Given only a normal vector, finds a valid tangent.
//
// This uses the technique from "Improved accuracy when building an
// orthonormal basis" by Nelson Max, https://jcgt.org/published/0006/01/02.
// Any tangent-generating algorithm must produce at least one discontinuity
// when operating on a sphere (due to the hairy ball theorem); this has a
// small ring-shaped discontinuity at normal.z == -0.99998796.
static glm::vec4 make_fast_tangent(const glm::vec3& n) {
  if (n.z < -0.99998796F)  // Handle the singularity
  {
    return glm::vec4(0.0F, -1.0F, 0.0F, 1.0F);
  }
  const float a = 1.0F / (1.0F + n.z);
  const float b = -n.x * n.y * a;
  return glm::vec4(1.0F - n.x * n.x * a, b, -n.x, 1.0F);
}

static bool try_parse_mesh_colour(const tinygltf::Model& gltf_scene_root,
                                  const tinygltf::Primitive& gltf_primitive,
                                  mesh_asset& mesh_asset);

static void try_parse_uvs(const tinygltf::Model& gltf_scene_root,
                          const tinygltf::Primitive& gltf_primitive,
                          mesh_asset& out_mesh_asset);
static void create_texcoords(const mesh_asset& mesh_asset,
                             std::vector<glm::vec2>& out_tex_coords);

static void try_parse_normals(const tinygltf::Model& gltf_scene_root,
                              const tinygltf::Primitive& gltf_primitive,
                              mesh_asset& out_mesh_asset);
static void create_normals(const mesh_asset& mesh_asset,
                           std::vector<glm::vec3>& out_normals);

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
  mesh_asset.m_material_handle = found_material_it == material_map.end()
                                     ? asset_handle::invalid()
                                     : found_material_it->second;

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

  if (!try_parse_mesh_colour(gltf_scene_root, gltf_primitive, mesh_asset)) {
    create_mesh_colour(gltf_scene_root, gltf_primitive, mesh_asset);
  }

  return mesh_asset;
}

bool try_parse_indices(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Primitive& gltf_primitive,
                       mesh_asset& mesh_asset) {
  if (gltf_primitive.indices > -1) {
    const tinygltf::Accessor& index_accessor =
        gltf_scene_root.accessors[gltf_primitive.indices];
    mesh_asset.m_indices.reserve(static_cast<uint32_t>(index_accessor.count));

    auto lambda = [&gltf_scene_root, &index_accessor,
                   &mesh_asset]<typename T>() {
      std::vector<T> indices;
      indices.resize(index_accessor.count);

      tinygltf::utils::copy_accessor_data(
          indices, 0, gltf_scene_root, index_accessor, 0, index_accessor.count);

      std::move(indices.begin(), indices.end(),
                std::back_inserter(mesh_asset.m_indices));
    };

    switch (index_accessor.componentType) {
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
        lambda.operator()<std::uint32_t>();
      } break;
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
        lambda.operator()<std::uint16_t>();
      } break;
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
        lambda.operator()<std::uint8_t>();
      } break;
      default:
        WUNDER_ERROR_TAG("Asset", "Index component type %i not supported!\n",
                         index_accessor.componentType);
        return false;
    }

    return true;
  }

  // Primitive without indices, creating them
  const auto& accessor =
      gltf_scene_root
          .accessors[gltf_primitive.attributes.find("POSITION")->second];
  for (auto i = 0; i < accessor.count; i++) {
    mesh_asset.m_indices.push_back(i);
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

  mesh_asset.m_vertices.resize(positions.size());
  for (std::uint32_t i = 0; i < positions.size(); ++i) {
    auto& position = positions[i];
    mesh_asset.m_vertices[i].m_position = position;
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
    mesh_asset.m_bounding_box.m_min = glm::vec3(
        accessor.minValues[0], accessor.minValues[1], accessor.minValues[2]);
  } else {
    mesh_asset.m_bounding_box.m_min =
        glm::vec3(std::numeric_limits<float>::max());
    for (const auto& vertex : mesh_asset.m_vertices) {
      mesh_asset.m_bounding_box.insert(vertex.m_position);
    }
  }

  if (!accessor.maxValues.empty()) {
    mesh_asset.m_bounding_box.m_max = glm::vec3(
        accessor.maxValues[0], accessor.maxValues[1], accessor.maxValues[2]);
  } else {
    mesh_asset.m_bounding_box.m_max =
        glm::vec3(-std::numeric_limits<float>::max());
    for (const auto& vertex : mesh_asset.m_vertices) {
      mesh_asset.m_bounding_box.insert(vertex.m_position);
    }
  }
}

void try_parse_normals(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Primitive& gltf_primitive,
                       mesh_asset& out_mesh_asset) {
  std::vector<glm::vec3> normals;
  if (!tinygltf::utils::get_attribute<glm::vec3>(
          gltf_scene_root, gltf_primitive, normals, "NORMAL")) {
    create_normals(out_mesh_asset, normals);
    for (auto& normal : normals) {
      normal = glm::normalize(normal);
    }
  }

  AssertReturnIf(normals.size() != out_mesh_asset.m_vertices.size(), );

  for (uint32_t i = 0; i < normals.size(); ++i) {
    const auto& normal = normals[i];
    out_mesh_asset.m_vertices[i].m_normal = normal;
  }
}

void create_normals(const mesh_asset& mesh_asset,
                    std::vector<glm::vec3>& out_normals) {
  // Need to compute the normals
  std::vector<glm::vec3> geo_normal(mesh_asset.m_vertices.size());
  for (size_t i = 0; i < mesh_asset.m_indices.size(); i += 3) {
    uint32_t ind0 = mesh_asset.m_indices[i + 0];
    uint32_t ind1 = mesh_asset.m_indices[i + 1];
    uint32_t ind2 = mesh_asset.m_indices[i + 2];
    const auto& pos0 = mesh_asset.m_vertices[ind0].m_position;
    const auto& pos1 = mesh_asset.m_vertices[ind1].m_position;
    const auto& pos2 = mesh_asset.m_vertices[ind2].m_position;

    const auto v1 = glm::normalize(
        pos1 - pos0);  // Many normalize, but when objects are really small the
    const auto v2 = glm::normalize(
        pos2 -
        pos0);  // cross will go below nv_eps and the normal will be (0,0,0)
    const auto n = glm::cross(v1, v2);
    geo_normal[ind0] += n;
    geo_normal[ind1] += n;
    geo_normal[ind2] += n;
  }

  for (auto& n : geo_normal) {
    n = glm::normalize(n);
  }

  out_normals.insert(out_normals.end(), geo_normal.begin(), geo_normal.end());
}

void try_parse_uvs(const tinygltf::Model& gltf_scene_root,
                   const tinygltf::Primitive& gltf_primitive,
                   mesh_asset& out_mesh_asset) {
  std::vector<glm::vec2> tex_coords;
  bool texcoord_created = tinygltf::utils::get_attribute<glm::vec2>(
      gltf_scene_root, gltf_primitive, tex_coords, "TEXCOORD_0");
  if (!texcoord_created) {
    texcoord_created = tinygltf::utils::get_attribute<glm::vec2>(
        gltf_scene_root, gltf_primitive, tex_coords, "TEXCOORD");

    if (!texcoord_created) {
      create_texcoords(out_mesh_asset, tex_coords);
    }
  }

  for (uint32_t i = 0; i < tex_coords.size(); ++i) {
    auto& tex_coord = tex_coords[i];
    out_mesh_asset.m_vertices[i].m_texcoord = tex_coord;
  }
}

void create_texcoords(const mesh_asset& mesh_asset,
                      std::vector<glm::vec2>& out_tex_coords) {
  // Set them all to zero
  //      m_texcoords0.insert(m_texcoords0.end(), resultMesh.vertexCount,
  //      glm::vec2(0, 0));

  // Cube map projection
  for (const auto& vertex : mesh_asset.m_vertices) {
    const auto& pos = vertex.m_position;
    float absX = fabs(pos.x);
    float absY = fabs(pos.y);
    float absZ = fabs(pos.z);

    int isXPositive = pos.x > 0 ? 1 : 0;
    int isYPositive = pos.y > 0 ? 1 : 0;
    int isZPositive = pos.z > 0 ? 1 : 0;

    float maxAxis{}, uc{},
        vc{};  // Zero-initialize in case pos = {NaN, NaN, NaN}

    // POSITIVE X
    if (isXPositive && absX >= absY && absX >= absZ) {
      // u (0 to 1) goes from +z to -z
      // v (0 to 1) goes from -y to +y
      maxAxis = absX;
      uc = -pos.z;
      vc = pos.y;
    }
    // NEGATIVE X
    if (!isXPositive && absX >= absY && absX >= absZ) {
      // u (0 to 1) goes from -z to +z
      // v (0 to 1) goes from -y to +y
      maxAxis = absX;
      uc = pos.z;
      vc = pos.y;
    }
    // POSITIVE Y
    if (isYPositive && absY >= absX && absY >= absZ) {
      // u (0 to 1) goes from -x to +x
      // v (0 to 1) goes from +z to -z
      maxAxis = absY;
      uc = pos.x;
      vc = -pos.z;
    }
    // NEGATIVE Y
    if (!isYPositive && absY >= absX && absY >= absZ) {
      // u (0 to 1) goes from -x to +x
      // v (0 to 1) goes from -z to +z
      maxAxis = absY;
      uc = pos.x;
      vc = pos.z;
    }
    // POSITIVE Z
    if (isZPositive && absZ >= absX && absZ >= absY) {
      // u (0 to 1) goes from -x to +x
      // v (0 to 1) goes from -y to +y
      maxAxis = absZ;
      uc = pos.x;
      vc = pos.y;
    }
    // NEGATIVE Z
    if (!isZPositive && absZ >= absX && absZ >= absY) {
      // u (0 to 1) goes from +x to -x
      // v (0 to 1) goes from -y to +y
      maxAxis = absZ;
      uc = -pos.x;
      vc = pos.y;
    }

    // Convert range from -1 to 1 to 0 to 1
    float u = 0.5f * (uc / maxAxis + 1.0f);
    float v = 0.5f * (vc / maxAxis + 1.0f);

    out_tex_coords.emplace_back(u, v);
  }
}

void try_parse_mesh_tangents(const tinygltf::Model& gltf_scene_root,
                             const tinygltf::Primitive& gltf_primitive,
                             mesh_asset& out_mesh_asset) {
  std::vector<glm::vec4> tangents;
  if (!tinygltf::utils::get_attribute<glm::vec4>(
          gltf_scene_root, gltf_primitive, tangents, "TANGENT")) {
    create_tangents(out_mesh_asset, tangents);
  }

  for (uint32_t i = 0; i < tangents.size(); ++i) {
    auto& tangent = tangents[i];
    out_mesh_asset.m_vertices[i].m_tangent = tangent;
  }
}

void create_tangents(const mesh_asset& mesh_asset,
                     std::vector<glm::vec4>& out_tangents) {
  std::size_t vertices_count = mesh_asset.m_vertices.size();
  std::vector<glm::vec3> tangent(vertices_count);
  std::vector<glm::vec3> bitangent(vertices_count);

  // Current implementation
  // http://foundationsofgameenginedev.com/FGED2-sample.pdf
  for (size_t i = 0; i < mesh_asset.m_indices.size(); i += 3) {
    // local index
    uint32_t i0 = mesh_asset.m_indices[i + 0];
    uint32_t i1 = mesh_asset.m_indices[i + 1];
    uint32_t i2 = mesh_asset.m_indices[i + 2];
    AssertContinueUnless(i0 < vertices_count);
    AssertContinueUnless(i1 < vertices_count);
    AssertContinueUnless(i2 < vertices_count);

    const auto& p0 = mesh_asset.m_vertices[i0];
    const auto& p1 = mesh_asset.m_vertices[i1];
    const auto& p2 = mesh_asset.m_vertices[i2];

    const auto& uv0 = p0.m_texcoord;
    const auto& uv1 = p1.m_texcoord;
    const auto& uv2 = p2.m_texcoord;

    glm::vec3 e1 = p1.m_position - p0.m_position;
    glm::vec3 e2 = p2.m_position - p0.m_position;

    glm::vec2 duvE1 = uv1 - uv0;
    glm::vec2 duvE2 = uv2 - uv0;

    float r = 1.0F;
    float a = duvE1.x * duvE2.y - duvE2.x * duvE1.y;
    if (fabs(a) > 0)  // Catch degenerated UV
    {
      r = 1.0f / a;
    }

    glm::vec3 t = (e1 * duvE2.y - e2 * duvE1.y) * r;
    glm::vec3 b = (e2 * duvE1.x - e1 * duvE2.x) * r;

    tangent[i0] += t;
    tangent[i1] += t;
    tangent[i2] += t;

    bitangent[i0] += b;
    bitangent[i1] += b;
    bitangent[i2] += b;
  }

  for (uint32_t a = 0; a < vertices_count; a++) {
    const auto& t = tangent[a];
    const auto& b = bitangent[a];
    const auto& n = mesh_asset.m_vertices[a].m_normal;

    // Gram-Schmidt orthogonalize
    glm::vec3 otangent = glm::normalize(t - (glm::dot(n, t) * n));

    // In case the tangent is invalid
    if (otangent == glm::vec3(0, 0, 0)) {
      otangent = glm::vec3(make_fast_tangent(n));
    }

    // Calculate handedness
    float handedness = (glm::dot(glm::cross(n, t), b) <= 0.0F) ? 1.0F : -1.0F;
    out_tangents.emplace_back(otangent.x, otangent.y, otangent.z, handedness);
  }
}

bool try_parse_mesh_colour(const tinygltf::Model& gltf_scene_root,
                           const tinygltf::Primitive& gltf_primitive,
                           mesh_asset& mesh_asset) {  // COLOR_0
  std::vector<glm::vec4> colors;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec4>(
                   gltf_scene_root, gltf_primitive, colors, "COLOR_0"),
               false);

  for (uint32_t i = 0; i < colors.size(); ++i) {
    auto& color = colors[i];
    mesh_asset.m_vertices[i].m_color = color;
  }

  return true;
}

void gltf_mesh_serializer::create_mesh_colour(
    const tinygltf::Model& model, const tinygltf::Primitive& primitive,
    wunder::mesh_asset& mesh_asset) {
  std::vector<glm::vec4> colours;

  colours.insert(colours.end(), mesh_asset.m_vertices.size(),
                 glm::vec4(1, 1, 1, 1));

  for (uint32_t i = 0; i < colours.size(); ++i) {
    auto& colour = colours[i];
    mesh_asset.m_vertices[i].m_color = colour;
  }
}
}  // namespace wunder
