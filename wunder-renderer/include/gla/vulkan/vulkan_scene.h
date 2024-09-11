//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_VULKAN_SCENE_H
#define WUNDER_VULKAN_SCENE_H

#include <glad/vulkan.h>

#include <vector>

#include "scene/scene_types.h"
#include "gla/vulkan/vulkan_types.h"

namespace wunder {
struct camera_component;
struct material_component;
struct mesh_component;
struct transform_component;
struct light_component;
struct texture_component;
struct scene_asset;

class vulkan_resource {
};

class vulkan_mesh_resource : vulkan_resource{
  vulkan_buffer m_vertex_buffer;
  vulkan_buffer m_index_buffer;

};

class vulkan_scene {
 public:
  void load_scene(scene_asset& asset);
 private:
  std::vector<vulkan_resource> binded_resources;
  };
}  // namespace wunder

#endif  // WUNDER_VULKAN_SCENE_H
