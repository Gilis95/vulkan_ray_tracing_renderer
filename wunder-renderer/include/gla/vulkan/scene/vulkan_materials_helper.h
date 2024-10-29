#ifndef WUNDER_VULKAN_MATERIALS_HELPER_H
#define WUNDER_VULKAN_MATERIALS_HELPER_H

#include "assets/asset_types.h"

namespace wunder {
struct material_asset;
struct mesh_asset;
class vulkan_materials_helper {
 public:
  [[nodiscard]] static assets<material_asset> extract_material_assets(
      assets<mesh_asset>& mesh_assets);
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_MATERIALS_HELPER_H
