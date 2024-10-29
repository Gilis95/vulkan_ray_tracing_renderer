//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_VULKAN_SCENE_H
#define WUNDER_VULKAN_SCENE_H

#include <vector>

#include "core/non_copyable.h"
#include "core/wunder_memory.h"

namespace wunder {
class vulkan_mesh_scene_node;
class scene_asset;
class vulkan_top_level_acceleration_structure;

class vulkan_scene : public non_copyable {
 public:
  vulkan_scene();
  ~vulkan_scene();
 public:
  vulkan_scene(vulkan_scene&&);
  vulkan_scene& operator=(vulkan_scene&&) noexcept ;

 public:
  void load_scene(scene_asset& asset);
 private:
  std::vector<vulkan_mesh_scene_node> m_mesh_nodes;
  unique_ptr<vulkan_top_level_acceleration_structure> m_acceleration_structure;

};
}  // namespace wunder

#endif  // WUNDER_VULKAN_SCENE_H
