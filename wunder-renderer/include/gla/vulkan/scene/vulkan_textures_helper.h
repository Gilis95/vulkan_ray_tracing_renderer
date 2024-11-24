#ifndef WUNDER_VULKAN_TEXTURES_HELP_H
#define WUNDER_VULKAN_TEXTURES_HELP_H

#include "assets/asset_types.h"
#include "core/vector_map.h"
#include "core/wunder_memory.h"

namespace wunder {
struct texture_asset;
struct material_asset;
class vulkan_texture;

class vulkan_textures_helper {
 public:
  static vector_map<asset_handle, const_ref<texture_asset>>
  extract_texture_assets(
      vector_map<asset_handle, const_ref<material_asset>>& material_assets);

  static std::vector<unique_ptr<vulkan_texture>> create_texture_buffers(
      const vector_map<asset_handle, const_ref<texture_asset>>& texture_assets);
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_TEXTURES_HELP_H
