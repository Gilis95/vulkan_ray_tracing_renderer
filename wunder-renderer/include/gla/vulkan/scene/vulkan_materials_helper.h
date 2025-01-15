#ifndef WUNDER_VULKAN_MATERIALS_HELPER_H
#define WUNDER_VULKAN_MATERIALS_HELPER_H

#include "assets/asset_types.h"
#include "include/assets/material_asset.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"

namespace wunder {
struct mesh_asset;
struct texture_asset;

namespace vulkan {
class materials_helper {
 public:
  [[nodiscard]] static assets<material_asset> extract_material_assets(
      assets<mesh_asset>& mesh_assets);

  [[nodiscard]] static unique_ptr<storage_buffer> create_material_buffer(
      const assets<material_asset>& materials,
      const assets<texture_asset>& texture_assets);

 private:
  static const material_asset& get_default_material();
};
}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_MATERIALS_HELPER_H
