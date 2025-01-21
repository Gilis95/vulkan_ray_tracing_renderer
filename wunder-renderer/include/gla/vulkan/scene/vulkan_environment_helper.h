#ifndef WUNDER_VULKAN_ENVIRONMENT_HELPER_H
#define WUNDER_VULKAN_ENVIRONMENT_HELPER_H

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"
#include "gla/vulkan/vulkan_texture_fwd.h"

namespace wunder::vulkan {
class vulkan_environment_helper final {
 public:
  [[nodsicard]] static unique_ptr<uniform_buffer>
  create_sky_and_sun_properties();
  static unique_ptr<sampled_texture> create_environment_texture();
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_ENVIRONMENT_HELPER_H
