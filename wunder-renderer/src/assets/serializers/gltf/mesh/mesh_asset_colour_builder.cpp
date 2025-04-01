#include <glm/vec4.hpp>
#include <vector>

#include "assets/asset.h"
#include "assets/mesh_asset.h"
#include "assets/serializers/gltf/mesh/mesh_asset_colour_builder.h"
#include "core/wunder_macros.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
mesh_asset_colour_builder::mesh_asset_colour_builder(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, wunder::mesh_asset& mesh_asset)
    : m_gltf_scene_root(gltf_scene_root),
      m_gltf_primitive(gltf_primitive),
      m_out_mesh_asset(mesh_asset) {}

bool mesh_asset_colour_builder::build() {  // COLOR_0
  std::vector<glm::vec4> colours;
  if (!tinygltf::utils::get_attribute<glm::vec4>(
          m_gltf_scene_root, m_gltf_primitive, colours, "COLOR_0")) {
    colours = create_mesh_colour();
  }

  for (uint32_t i = 0; i < colours.size(); ++i) {
    auto& colour = colours[i];
    m_out_mesh_asset.m_vertices[i].m_color = colour;
  }

  return true;
}

std::vector<glm::vec4> mesh_asset_colour_builder::create_mesh_colour() const {
  std::vector<glm::vec4> colours;

  colours.insert(colours.end(), m_out_mesh_asset.m_vertices.size(),
                 glm::vec4(1, 1, 1, 1));
  return colours;
}

}  // namespace wunder