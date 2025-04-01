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

class texture_resource_creator {
 public:
  texture_resource_creator();

 public:
  std::vector<unique_ptr<sampled_texture>> create_texture_buffers(
      const assets<material_asset>& material_assets);

 public:
  [[nodiscard]] const assets<texture_asset>& get_texture_assets() const {
    return m_texture_assets;
  }

 private:
  void extract_texture_assets(const assets<material_asset>& material_assets);

 private:
  assets<texture_asset> m_texture_assets;
};
}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_TEXTURES_HELP_H
