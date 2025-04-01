#include "assets/mesh_asset.h"
#include "assets/serializers/gltf/mesh/mesh_asset_uvs_builder.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {

mesh_asset_uvs_builder::mesh_asset_uvs_builder(const tinygltf::Model& gltf_scene_root,
                                   const tinygltf::Primitive& gltf_primitive,
                                   mesh_asset& out_mesh_asset)
    : m_gltf_scene_root(gltf_scene_root),
      m_gltf_primitive(gltf_primitive),
      m_out_mesh_asset(out_mesh_asset) {}

void mesh_asset_uvs_builder::build() {
  std::vector<glm::vec2> tex_coords;
  bool texcoord_created = tinygltf::utils::get_attribute<glm::vec2>(
      m_gltf_scene_root, m_gltf_primitive, tex_coords, "TEXCOORD_0");
  if (!texcoord_created) {
    texcoord_created = tinygltf::utils::get_attribute<glm::vec2>(
        m_gltf_scene_root, m_gltf_primitive, tex_coords, "TEXCOORD");

    if (!texcoord_created) {
      create_texcoords(tex_coords);
    }
  }

  for (uint32_t i = 0; i < tex_coords.size(); ++i) {
    auto& tex_coord = tex_coords[i];
    m_out_mesh_asset.m_vertices[i].m_texcoord = tex_coord;
  }
}

void mesh_asset_uvs_builder::create_texcoords(
    std::vector<glm::vec2>& out_tex_coords) const {
  // Cube map projection
  for (const auto& vertex : m_out_mesh_asset.m_vertices) {
    const auto& pos = vertex.m_position;
    float absX = std::abs(pos.x);
    float absY = std::abs(pos.y);
    float absZ = std::abs(pos.z);

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

}  // namespace wunder