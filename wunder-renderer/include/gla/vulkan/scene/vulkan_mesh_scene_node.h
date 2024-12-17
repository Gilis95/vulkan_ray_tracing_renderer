//
// Created by christian on 9/12/24.
//

#ifndef WUNDER_VULKAN_MESH_NODE_H
#define WUNDER_VULKAN_MESH_NODE_H

#include "core/wunder_memory.h"
#include "glm/detail/type_mat4x4.hpp"
#include "resources/shaders/host_device.h"

namespace wunder {
namespace vulkan {

struct vulkan_mesh;
class renderer;

struct vulkan_mesh_scene_node {
 public:
  shared_ptr<vulkan_mesh> m_mesh;
  glm::mat4 m_model_matrix;

};
}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_MESH_NODE_H
