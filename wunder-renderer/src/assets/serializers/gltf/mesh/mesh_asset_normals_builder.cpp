#include "assets/serializers/gltf/mesh/mesh_asset_normals_builder.h"

#include <glm/geometric.hpp>

#include "assets/mesh_asset.h"
#include "core/wunder_macros.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
mesh_asset_normals_builder::mesh_asset_normals_builder(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, mesh_asset& out_mesh_asset)
    : m_gltf_scene_root(gltf_scene_root),
      m_gltf_primitive(gltf_primitive),
      m_out_mesh_asset(out_mesh_asset) {}

void mesh_asset_normals_builder::build() {
  std::vector<glm::vec3> normals;
  if (!tinygltf::utils::get_attribute<glm::vec3>(
          m_gltf_scene_root, m_gltf_primitive, normals, "NORMAL")) {
    create_normals(normals);
  }

  AssertReturnIf(normals.size() != m_out_mesh_asset.m_vertices.size(), );

  for (uint32_t i = 0; i < normals.size(); ++i) {
    const auto& normal = normals[i];
    m_out_mesh_asset.m_vertices[i].m_normal = glm::normalize(normal);
  }
}

void mesh_asset_normals_builder::create_normals(
    std::vector<glm::vec3>& out_normals) {
  // Need to compute the normals
  std::vector<glm::vec3> geo_normal(m_out_mesh_asset.m_vertices.size());
  for (size_t i = 0; i < m_out_mesh_asset.m_indices.size(); i += 3) {
    uint32_t ind0 = m_out_mesh_asset.m_indices[i + 0];
    uint32_t ind1 = m_out_mesh_asset.m_indices[i + 1];
    uint32_t ind2 = m_out_mesh_asset.m_indices[i + 2];
    const auto& pos0 = m_out_mesh_asset.m_vertices[ind0].m_position;
    const auto& pos1 = m_out_mesh_asset.m_vertices[ind1].m_position;
    const auto& pos2 = m_out_mesh_asset.m_vertices[ind2].m_position;

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

}  // namespace wunder