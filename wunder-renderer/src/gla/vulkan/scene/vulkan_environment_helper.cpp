#include "gla/vulkan/scene/vulkan_environment_helper.h"

#include "assets/asset_manager.h"
#include "core/project.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_texture.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
unique_ptr<uniform_buffer>
vulkan_environment_helper::create_sky_and_sun_properties() {
  SunAndSky sun_and_sky{
      {1, 1, 1},            // rgb_unit_conversion;
      0.0000101320f,        // multiplier;
      0.0f,                 // haze;
      0.0f,                 // redblueshift;
      1.0f,                 // saturation;
      0.0f,                 // horizon_height;
      {0.4f, 0.4f, 0.4f},   // ground_color;
      0.1f,                 // horizon_blur;
      {0.0, 0.0, 0.01f},    // night_color;
      0.8f,                 // sun_disk_intensity;
      {0.00, 0.78, 0.62f},  // sun_direction;
      5.0f,                 // sun_disk_scale;
      1.0f,                 // sun_glow_intensity;
      1,                    // y_is_up;
      1,                    // physically_scaled_sun;
      0,                    // in_use;
  };

  return std::make_unique<uniform_device_buffer>(
      descriptor_build_data{.m_enabled = true,
                            .m_descriptor_name = "_SSBuffer"},
      &sun_and_sky, sizeof(SunAndSky), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

unique_ptr<sampled_texture>
vulkan_environment_helper::create_environment_texture() {
  // TODO:: first asset is being retrieved, it should be pointed out in the
  // scene, which one to be used
  auto& asset_manager = project::instance().get_asset_manager();
  assets<environment_texture_asset> assets =
      asset_manager.find_assets<environment_texture_asset>();

  AssertReturnIf(assets.empty(), nullptr);

  const_ref<environment_texture_asset>& first_environment_texture =
      assets.begin()->second;
  return std::make_unique<sampled_texture>(
      descriptor_build_data{.m_enabled = true,
                            .m_descriptor_name = "environmentTexture"},
      first_environment_texture.get());
}
}  // namespace wunder::vulkan