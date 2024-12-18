#ifndef WUNDER_ASSET_STORAGE_H
#define WUNDER_ASSET_STORAGE_H

#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

#include "asset.h"
#include "assets/asset_types.h"
#include "core/vector_map.h"
#include "core/wunder_macros.h"

namespace wunder {
class asset_storage {
 public:
  asset_handle add_asset(asset&& asset);

  template <typename asset_type>
  optional_const_ref<asset_type> find_asset(asset_handle handle) const;

  template <typename asset_type>
  vector_map<asset_handle, const_ref<asset_type>> find_assets_of() const;

 public:
  std::unordered_map<asset_handle, asset> m_assets;
};

template <typename asset_type>
optional_const_ref<asset_type> asset_storage::find_asset(
    asset_handle handle) const {
  auto found_asset_it = m_assets.find(handle);
  ReturnIf(found_asset_it == m_assets.end(), std::nullopt);

  const asset& found_asset = found_asset_it->second;
  const asset_type* const asset_of_type = std::get_if<asset_type>(&found_asset);
  ReturnUnless(asset_of_type, std::nullopt);

  return *asset_of_type;
}

template <typename asset_type>
vector_map<asset_handle, const_ref<asset_type>>
asset_storage::find_assets_of() const {
  vector_map<asset_handle, const_ref<asset_type>> result;
  for (const auto& [asset_id, asset] : m_assets) {
    const asset_type* const asset_of_type =
        std::get_if<asset_type>(&asset);
    ContinueUnless(asset_of_type);

    result.push_back(asset_id, asset);
  }
  return result;
}

}  // namespace wunder
#endif  // WUNDER_ASSET_STORAGE_H
