#include "gla/vulkan/scene/vulkan_materials_helper.h"

#include <functional>
#include <numeric>
#include <unordered_set>

#include "assets/asset_manager.h"
#include "core/project.h"
#include "core/vector_map.h"

namespace wunder {

assets<material_asset> vulkan_materials_helper::extract_material_assets(
    assets<mesh_asset>& mesh_assets) {
  auto& asset_manager = project::instance().get_asset_manager();

  std::unordered_set<asset_handle> material_ids;
  for (auto& [asset_id, mesh] : mesh_assets) {
    material_ids.emplace(mesh.get().m_material_handle);
  }

  vector_map<asset_handle, std::reference_wrapper<const material_asset>>
      result = asset_manager.find_assets<material_asset>(material_ids.begin(),
                                                         material_ids.end());

  return result;
}
}  // namespace wunder