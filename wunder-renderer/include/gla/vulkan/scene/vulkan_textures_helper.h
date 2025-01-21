#ifndef WUNDER_VULKAN_TEXTURES_HELP_H
#define WUNDER_VULKAN_TEXTURES_HELP_H

#include "assets/asset_types.h"
#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_texture_fwd.h"
namespace wunder {
struct texture_asset;
struct material_asset;

namespace vulkan {

class textures_helper {
 public:
  static assets<texture_asset> extract_texture_assets(
      const assets<material_asset>& material_assets);

  static std::vector<unique_ptr<sampled_texture>> create_texture_buffers(
      const assets<texture_asset>& texture_assets);
};
}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_TEXTURES_HELP_H
