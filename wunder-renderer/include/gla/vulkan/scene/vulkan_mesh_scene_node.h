//
// Created by christian on 9/12/24.
//

#ifndef WUNDER_VULKAN_MESH_NODE_H
#define WUNDER_VULKAN_MESH_NODE_H

#include "core/wunder_memory.h"
#include "glm/detail/type_mat4x4.hpp"

namespace wunder{
struct vulkan_mesh;

struct vulkan_mesh_scene_node {
  shared_ptr<vulkan_mesh> m_mesh;
  glm::mat4 m_model_matrix;
};
}
#endif  // WUNDER_VULKAN_MESH_NODE_H
