#ifndef WUNDER_MESH_ASSET_H
#define WUNDER_MESH_ASSET_H

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

#include "core/aabb.h"
#include "assets/asset_types.h"

namespace wunder {
struct vertex
{
  glm::vec3 m_position;
  glm::vec3 m_normal;
  glm::vec4 m_tangent;
  glm::vec3 m_binormal;
  glm::vec2 m_texcoord;
  glm::vec4 m_color;
};


struct mesh_asset {
  std::vector<vertex> m_verticies;
  std::vector<std::uint32_t> m_indecies;
  asset_handle m_material_handle;
  aabb          m_bounding_box;
};

}  // namespace wunder
#endif  // WUNDER_MESH_ASSET_H
