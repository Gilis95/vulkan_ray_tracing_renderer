#ifndef WUNDER_TEXTURE_ASSET_H
#define WUNDER_TEXTURE_ASSET_H

#include <cstdint>
#include <limits>
#include <vector>

namespace wunder {

enum class texture_filter_type { NEAREST, LINEAR };

enum class mipmap_mode_type { NEAREST, LINEAR };

enum class address_mode_type { CLAMP_TO_EDGE, MIRRORED_REPEAT, REPEAT };

struct texture_asset {
  std::vector<unsigned char> m_texture_data;
  texture_filter_type m_mag_filter = texture_filter_type::LINEAR;
  texture_filter_type m_min_filter = texture_filter_type::LINEAR;
  mipmap_mode_type m_mipmap_mode = mipmap_mode_type::LINEAR;
  address_mode_type m_address_mode_u = address_mode_type::CLAMP_TO_EDGE;
  address_mode_type m_address_mode_v = address_mode_type::CLAMP_TO_EDGE;
  std::uint32_t m_max_lod = std::numeric_limits<uint32_t>::max();
};
}  // namespace wunder
#endif  // WUNDER_TEXTURE_ASSET_H
