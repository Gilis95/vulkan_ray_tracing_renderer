//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_VULKAN_SCENE_H
#define WUNDER_VULKAN_SCENE_H

#include <vector>

#include "core/non_copyable.h"
#include "core/wunder_memory.h"

namespace wunder {
class scene_asset;

namespace vulkan {
class vulkan_mesh_scene_node;
class top_level_acceleration_structure;
class texture;
class buffer;
class renderer;

class scene : public non_copyable {
 public:
  scene();
  ~scene();

 public:
  scene(scene&&);
  scene& operator=(scene&&) noexcept;

 public:
  void load_scene(scene_asset& asset);
  void bind(renderer& renderer);

 private:
  std::vector<unique_ptr<texture>> m_bound_textures;
  unique_ptr<buffer> m_material_buffer;

  std::vector<vulkan_mesh_scene_node> m_mesh_nodes;
  unique_ptr<top_level_acceleration_structure> m_acceleration_structure;
};
}  // namespace vulkan
}  // namespace wunder

#endif  // WUNDER_VULKAN_SCENE_H
