//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_VULKAN_SCENE_H
#define WUNDER_VULKAN_SCENE_H

#include <vector>

#include "core/non_copyable.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"
#include "gla/vulkan/vulkan_texture_fwd.h"

namespace wunder {
class scene_asset;

namespace vulkan {
struct vulkan_environment;
class top_level_acceleration_structure_build_info;
class descriptor_set_manager;
class vulkan_mesh_scene_node;
class top_level_acceleration_structure;

class scene : public non_copyable {
 public:
  scene();
  ~scene();

 public:
  scene(scene&&);
  scene& operator=(scene&&) noexcept;

 public:
  void load_scene(const scene_asset& asset);
  void collect_descriptors(descriptor_set_manager& target);

  [[nodiscard]] const vulkan_environment& get_environment_texture() const ;

  [[nodiscard]] std::uint64_t get_lights_count() const { return m_lights_count; };


 private:
  std::vector<unique_ptr<sampled_texture>> m_bound_textures;
  unique_ptr<storage_buffer> m_material_buffer;
  unique_ptr<storage_buffer> m_light_buffer;
  unique_ptr<storage_buffer> m_mesh_instance_data_buffer;

  unique_ptr<uniform_buffer> m_sun_and_sky_properties_buffer;
  unique_ptr<vulkan_environment> m_environment_textures;

  std::vector<vulkan_mesh_scene_node> m_mesh_nodes;
  unique_ptr<top_level_acceleration_structure> m_acceleration_structure;
  std::vector<top_level_acceleration_structure_build_info> m_acceleration_structure_build_info;

  std::uint64_t m_lights_count;
};
}  // namespace vulkan
}  // namespace wunder

#endif  // WUNDER_VULKAN_SCENE_H
