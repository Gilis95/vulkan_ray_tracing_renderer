#ifndef WUNDER_VULKAN_LIGHTS_HELPER_H
#define WUNDER_VULKAN_LIGHTS_HELPER_H

#include "assets/asset_types.h"
#include "assets/scene_node.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"
#include "resources/shaders/host_device.h"

struct Light;

namespace wunder {
struct light_asset;
}  // namespace wunder

namespace wunder::vulkan {
class lights_helper {
 public:
  [[nodiscard]] static unique_ptr<storage_buffer> create_light_buffer(
      const std::vector<ref<scene_node>>& light_nodes);
 private:
  [[nodiscard]] static assets<light_asset> extract_scene_light_data(
      const std::vector<ref<scene_node>>& light_nodes,
      vector_map<asset_handle, transform_component>& out_transformations);
  [[nodiscard]] static std::vector<Light> create_host_light_array(
      const vector_map<asset_handle, transform_component>& transformations,
      assets<light_asset>& light_assets);

  static void map_to_host_light(Light& host_light, const light_asset& light,
                                mat4 mat);
  static void map_host_light_specific_data(Light& host_light,
                                           const light_asset& light);
static light_asset& get_default_light_asset();
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_LIGHTS_HELPER_H
