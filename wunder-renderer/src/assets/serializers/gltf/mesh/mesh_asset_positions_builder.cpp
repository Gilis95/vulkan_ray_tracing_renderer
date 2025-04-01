#include <glm/vec3.hpp>
#include <vector>

#include "assets/mesh_asset.h"
#include "assets/serializers/gltf/mesh/mesh_asset_positions_builder.h"
#include "core/wunder_macros.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
mesh_asset_positions_builder::mesh_asset_positions_builder(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, mesh_asset& mesh_asset)
    : m_gltf_scene_root(gltf_scene_root),
      m_gltf_primitive(gltf_primitive),
      m_out_mesh_asset(mesh_asset) {}

bool mesh_asset_positions_builder::build() {
  std::vector<glm::vec3> positions;
  ReturnUnless(tinygltf::utils::get_attribute<glm::vec3>(
                   m_gltf_scene_root, m_gltf_primitive, positions, "POSITION"),
               false);

  m_out_mesh_asset.m_vertices.resize(positions.size());
  for (std::uint32_t i = 0; i < positions.size(); ++i) {
    auto& position = positions[i];
    m_out_mesh_asset.m_vertices[i].m_position = position;
  }

  parse_aabb();

  return true;
}

void mesh_asset_positions_builder::parse_aabb() {
  const auto& accessor =
      m_gltf_scene_root
          .accessors[m_gltf_primitive.attributes.find("POSITION")->second];

  if (!accessor.minValues.empty()) {
    m_out_mesh_asset.m_bounding_box.m_min = glm::vec3(
        accessor.minValues[0], accessor.minValues[1], accessor.minValues[2]);
  } else {
    m_out_mesh_asset.m_bounding_box.m_min =
        glm::vec3(std::numeric_limits<float>::max());
    for (const auto& vertex : m_out_mesh_asset.m_vertices) {
      m_out_mesh_asset.m_bounding_box.insert(vertex.m_position);
    }
  }

  if (!accessor.maxValues.empty()) {
    m_out_mesh_asset.m_bounding_box.m_max = glm::vec3(
        accessor.maxValues[0], accessor.maxValues[1], accessor.maxValues[2]);
  } else {
    m_out_mesh_asset.m_bounding_box.m_max =
        glm::vec3(-std::numeric_limits<float>::max());
    for (const auto& vertex : m_out_mesh_asset.m_vertices) {
      m_out_mesh_asset.m_bounding_box.insert(vertex.m_position);
    }
  }
}

}  // namespace wunder