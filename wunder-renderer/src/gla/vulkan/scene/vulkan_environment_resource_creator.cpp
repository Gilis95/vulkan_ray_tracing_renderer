#include <numeric>

#include "assets/asset_manager.h"
#include "assets/texture_asset.h"
#include "core/project.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/scene/vulkan_environment.h"
#include "gla/vulkan/scene/vulkan_environment_resource_creator.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_texture.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
namespace {
// CIE luminance
float luminance(const float* color) {
  return color[0] * 0.2126f + color[1] * 0.7152f + color[2] * 0.0722f;
}

}  // namespace

unique_ptr<uniform_buffer>
vulkan_environment_resource_creator::create_sky_and_sun_properties() {
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
      2.0f,                 // sun_disk_scale;
      2.0f,                 // sun_glow_intensity;
      1,                    // y_is_up;
      1,                    // physically_scaled_sun;
      0,                    // in_use;
  };

  return std::make_unique<uniform_device_buffer>(
      descriptor_build_data{.m_enabled = true,
                            .m_descriptor_name = "_SSBuffer"},
      &sun_and_sky, sizeof(SunAndSky), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

vulkan_environment vulkan_environment_resource_creator::create_environment_texture() {
  vulkan_environment environment{};

  // TODO:: first asset is being retrieved, it should be pointed out in the
  // scene, which one to be used
  auto& asset_manager = project::instance().get_asset_manager();
  assets<environment_texture_asset> assets =
      asset_manager.find_assets<environment_texture_asset>();

  AssertReturnIf(assets.empty(), environment);

  const_ref<environment_texture_asset>& first_environment_texture =
      assets.begin()->second;
  environment.m_image = std::make_unique<sampled_texture>(
      descriptor_build_data{.m_enabled = true,
                            .m_descriptor_name = "environmentTexture"},
      first_environment_texture.get());
  // Needed acceleration metadata
  create_environment_accel(first_environment_texture.get(), environment);

  return environment;
}

//--------------------------------------------------------------------------------------------------
// Create acceleration data for importance sampling
// See:  https://arxiv.org/pdf/1901.05423.pdf
void vulkan_environment_resource_creator::create_environment_accel(
    const environment_texture_asset& asset,
    vulkan_environment& out_environment_data) {
  std::visit(
      overloaded{
          [](const std::vector<unsigned char>& /*pixels*/) -> void {
            WUNDER_ERROR(
                "Environment map components couldn't be with size less than "
                "32bits");
          },
          [&asset,
           &out_environment_data](const std::vector<float>& pixels) -> void {
            create_environment_accel(asset, out_environment_data, pixels);
          }},
      asset.m_texture_data.m_data);
}

void vulkan_environment_resource_creator::create_environment_accel(
    const environment_texture_asset& asset,
    vulkan_environment& out_environment_data,
    const std::vector<float>& pixels) {
  const uint32_t rx = asset.m_width;
  const uint32_t ry = asset.m_height;

  // Create importance sampling data
  std::vector<EnvAccel> env_accels(rx * ry);
  std::vector<float> importance_data(rx * ry);

  float cos_theta0 = 1.0f;
  const float step_phi = float(2.0 * M_PI) / float(rx);
  const float step_theta = float(M_PI) / float(ry);
  double total = 0;

  // For each texel of the environment map, we compute the related
  // solid angle subtended by the texel, and store the weighted
  // luminance in importance_data, representing the amount of energy
  // emitted through each texel. Also compute the average CIE
  // luminance to drive the tonemapping of the final image
  for (uint32_t y = 0; y < ry; ++y) {
    const float theta1 = float(y + 1) * step_theta;
    const float cos_theta1 = std::cos(theta1);
    const float area = (cos_theta0 - cos_theta1) * step_phi;  // solid angle
    cos_theta0 = cos_theta1;

    for (uint32_t x = 0; x < rx; ++x) {
      const uint32_t idx = y * rx + x;
      const uint32_t idx4 = idx * 4;
      float cie_luminance = luminance(&pixels[idx4]);
      importance_data[idx] =
          area *
          std::max(pixels[idx4], std::max(pixels[idx4 + 1], pixels[idx4 + 2]));
      total += cie_luminance;
    }
  }

  out_environment_data.m_acceleration_data.m_average_luminance =
      static_cast<float>(total) / static_cast<float>(rx * ry);

  // Build the alias map, which aims at creating a set of texel
  // couples so that all couples emit roughly the same amount of
  // energy. To this aim, each smaller radiance texel will be assigned
  // an "alias" with higher emitted radiance As a byproduct this
  // function also returns the integral of the radiance emitted by the
  // environment
  out_environment_data.m_acceleration_data.m_integral =
      build_alias_map(importance_data, env_accels);

  // We deduce the PDF of each texel by normalizing its emitted
  // radiance by the radiance integral
  const float invEnvIntegral =
      1.0f / out_environment_data.m_acceleration_data.m_integral;
  for (uint32_t i = 0; i < rx * ry; ++i) {
    const uint32_t idx4 = i * 4;
    env_accels[i].pdf =
        std::max(pixels[idx4], std::max(pixels[idx4 + 1], pixels[idx4 + 2])) *
        invEnvIntegral;
  }

  // At runtime a texel will be uniformly chosen. Whether that texel
  // or its alias is selected depends on the relative emitted
  // radiances of the two texels. We store the PDF of the alias
  // together with the PDF of the first member, so that both PDFs are
  // available in a single lookup
  for (uint32_t i = 0; i < rx * ry; ++i) {
    const uint32_t aliasIdx = env_accels[i].alias;
    env_accels[i].aliasPdf = env_accels[aliasIdx].pdf;
  }

  out_environment_data.m_acceleration_data.m_buffer =
      std::make_unique<storage_device_buffer>(
          descriptor_build_data{.m_enabled = true,
                                .m_descriptor_name = "_EnvAccel"},
          env_accels.data(), env_accels.size() * sizeof(EnvAccel),
          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
}

//--------------------------------------------------------------------------------------------------
// Build alias map for the importance sampling: Each texel is associated to
// another texel, or alias, so that their combined intensities are a close as
// possible to the average of the environment map. This will later allow the
// sampling shader to uniformly select a texel in the environment, and select
// either that texel or its alias depending on their relative intensities
//
float vulkan_environment_resource_creator::build_alias_map(const std::vector<float>& data,
                                                 std::vector<EnvAccel>& accel) {
  auto size = static_cast<uint32_t>(data.size());

  // Compute the integral of the emitted radiance of the environment map
  // Since each element in data is already weighted by its solid angle
  // the integral is a simple sum
  float sum = std::accumulate(data.begin(), data.end(), 0.f);

  // For each texel, compute the ratio q between the emitted radiance of the
  // texel and the average emitted radiance over the entire sphere We also
  // initialize the aliases to identity, ie. each texel is its own alias
  auto f_size = static_cast<float>(size);
  float inverse_average = f_size / sum;
  for (uint32_t i = 0; i < size; ++i) {
    accel[i].q = data[i] * inverse_average;
    accel[i].alias = i;
  }

  // Partition the texels according to their emitted radiance ratio wrt.
  // average. Texels with a value q < 1 (ie. below average) are stored
  // incrementally from the beginning of the array, while texels emitting
  // higher-than-average radiance are stored from the end of the array
  std::vector<uint32_t> partition_table(size);
  uint32_t s = 0u;
  uint32_t large = size;
  for (uint32_t i = 0; i < size; ++i) {
    if (accel[i].q < 1.f) {
      partition_table[s++] = i;
    } else {
      partition_table[--large] = i;
    }
  }

  // Associate the lower-energy texels to higher-energy ones. Since the emission
  // of a high-energy texel may be vastly superior to the average,
  for (s = 0; s < large && large < size; ++s) {
    // Index of the smaller energy texel
    const uint32_t small_energy_index = partition_table[s];

    // Index of the higher energy texel
    const uint32_t high_energy_index = partition_table[large];

    // Associate the texel to its higher-energy alias
    accel[small_energy_index].alias = high_energy_index;

    // Compute the difference between the lower-energy texel and the average
    const float difference_with_average = 1.f - accel[small_energy_index].q;

    // The goal is to obtain texel couples whose combined intensity is close to
    // the average. However, some texels may have low energies, while others may
    // have very high intensity (for example a sunset: the sky is quite dark,
    // but the sun is still visible). In this case it may not be possible to
    // obtain a value close to average by combining only two texels. Instead, we
    // potentially associate a single high-energy texel to many smaller-energy
    // ones until the combined average is similar to the average of the
    // environment map. We keep track of the combined average by subtracting the
    // difference between the lower-energy texel and the average from the ratio
    // stored in the high-energy texel.
    accel[high_energy_index].q -= difference_with_average;

    // If the combined ratio to average of the higher-energy texel reaches 1, a
    // balance has been found between a set of low-energy texels and the
    // higher-energy one. In this case, we will use the next higher-energy texel
    // in the partition when processing the next texel.
    if (accel[high_energy_index].q < 1.0f) {
      ++large;
    }
  }
  // Return the integral of the emitted radiance. This integral will be used to
  // normalize the probability distribution function (PDF) of each pixel
  return sum;
}
}  // namespace wunder::vulkan