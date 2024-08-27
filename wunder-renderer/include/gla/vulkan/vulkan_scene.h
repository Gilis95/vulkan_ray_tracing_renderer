//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_VULKAN_SCENE_H
#define WUNDER_VULKAN_SCENE_H

#include <glad/vulkan.h>

#include <vector>

#include "scene/scene_types.h"

namespace wunder {
struct camera_component;
struct material_component;
struct mesh_component;
struct transform_component;
struct light_component;

class vulkan_resource {
  std::vector<VkBuffer> m_buffers;
};

class vulkan_scene {
 public:
  void start_binding();
  void finish_binding();

 public:
  void operator()(const camera_component& component);
  void operator()(const material_component& component);
  void operator()(const mesh_component&);
  void operator()(const transform_component& component);
  void operator()(const light_component& component);

 private:
  std::vector<vulkan_resource> binded_resources;
};
}  // namespace wunder

#endif  // WUNDER_VULKAN_SCENE_H
