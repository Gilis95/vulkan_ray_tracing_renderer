#ifndef WUNDER_VULKAN_ENVIRONMENT_HELPER_H
#define WUNDER_VULKAN_ENVIRONMENT_HELPER_H

#include <vector>

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"

namespace wunder {
struct environment_texture_asset;
}

namespace wunder::vulkan {
struct vulkan_environment;
}

struct EnvAccel;

namespace wunder::vulkan {
class vulkan_environment_resource_creator final {
 public:
  static unique_ptr<uniform_buffer>  create_sky_and_sun_properties();
  static unique_ptr<vulkan_environment> create_environment_texture();

 private:
  static float build_alias_map(const std::vector<float>& data,
                               std::vector<EnvAccel>& accel);
  static void create_environment_accel(
      const environment_texture_asset& asset,
      vulkan_environment& out_environment_data);
  static void create_environment_accel(const environment_texture_asset& asset,
                                       vulkan_environment& out_environment_data,
                                       const std::vector<float>& pixels);
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_ENVIRONMENT_HELPER_H
