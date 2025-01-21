#include "assets/serializers/environment_map_serializer.h"

#include <cstring>
#include <vector>

#include "assets/asset_storage.h"
#include "assets/texture_asset.h"

namespace wunder {
asset_serialization_result_codes environment_map_serializer::import_asset(
    input_data input, asset_storage& out_storage) {
  AssertReturnUnless(input.m_pixels_ptr,
                     asset_serialization_result_codes::error)

      std::vector<float>
          pixels;
  int32_t texture_elements_count =
      input.m_width * input.m_height * input.m_components;
  pixels.resize(texture_elements_count);

  std::memcpy(pixels.data(), input.m_pixels_ptr,
              texture_elements_count * sizeof(float));

  environment_texture_asset texture{std::move(pixels), input.m_width,
                                    input.m_height, std::nullopt};

  texture.m_sampler =
      texture_sampler{.m_mag_filter = texture_filter_type::LINEAR,
                      .m_min_filter = texture_filter_type::LINEAR,
                      .m_mipmap_mode = mipmap_mode_type::LINEAR,
                      .m_address_mode_u = address_mode_type::CLAMP_TO_EDGE,
                      .m_address_mode_v = address_mode_type::CLAMP_TO_EDGE};

  out_storage.add_asset(std::move(texture));

  return asset_serialization_result_codes::ok;
}
}  // namespace wunder