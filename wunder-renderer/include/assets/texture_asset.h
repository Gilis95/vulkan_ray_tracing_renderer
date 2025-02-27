#ifndef WUNDER_TEXTURE_ASSET_H
#define WUNDER_TEXTURE_ASSET_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include <cstdint>
#include <limits>
#include <optional>
#include <variant>
#include <vector>

namespace wunder {

enum class texture_filter_type { NEAREST, LINEAR };

enum class mipmap_mode_type { NEAREST, LINEAR };

enum class address_mode_type { CLAMP_TO_EDGE, MIRRORED_REPEAT, REPEAT };
enum class border_colour {
  FLOAT_TRANSPARENT_BLACK,
  INT_TRANSPARENT_BLACK,
  FLOAT_OPAQUE_BLACK,
  INT_OPAQUE_BLACK,
  FLOAT_OPAQUE_WHITE,
  INT_OPAQUE_WHITE,
};

struct texture_sampler {
  texture_filter_type m_mag_filter = texture_filter_type::LINEAR;
  texture_filter_type m_min_filter = texture_filter_type::LINEAR;
  mipmap_mode_type m_mipmap_mode = mipmap_mode_type::LINEAR;
  address_mode_type m_address_mode_u = address_mode_type::CLAMP_TO_EDGE;
  address_mode_type m_address_mode_v = address_mode_type::CLAMP_TO_EDGE;
  border_colour m_border_colour = border_colour::FLOAT_TRANSPARENT_BLACK;
};

struct texture_data {
  using data_type =
      std::variant<std::vector<unsigned char>, std::vector<float>>;

  data_type m_data;

  bool is_empty() const;
  size_t size() const;
  void copy_to(VmaAllocation& stagingBufferAllocation) const;
};

struct texture_asset {
  texture_data m_texture_data;
  int m_width, m_height;
  std::optional<texture_sampler> m_sampler;
  std::uint32_t m_max_lod = std::numeric_limits<uint32_t>::max();
};

struct environment_texture_asset : public texture_asset {};
}  // namespace wunder
#endif  // WUNDER_TEXTURE_ASSET_H
