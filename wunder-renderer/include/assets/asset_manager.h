//
// Created by christian on 9/10/24.
//

#ifndef WUNDER_ASSET_MANAGER_H
#define WUNDER_ASSET_MANAGER_H

#include <filesystem>

#include "assets/asset_storage.h"
#include "assets/asset_types.h"
namespace tinygltf {
class Model;
}
namespace wunder {
class asset_manager {
 public:
  asset_manager();
  ~asset_manager();

 public:
  asset_serialization_result_codes import_asset(
      const std::filesystem::path& asset);

 public:
  template <typename asset_type>
  std::optional<std::reference_wrapper<const asset_type>> find_asset(
      asset_handle handle) const;

  template <typename asset_type>
  [[nodiscard]] std::vector<
      std::reference_wrapper<const std::pair<asset_handle, asset_type>>>
  find_assets() const;

 private:
  asset_serialization_result_codes load_gltf_file(
      tinygltf::Model& gltf_model);

 private:
  asset_storage m_asset_storage;
};

template <typename asset_type>
std::optional<std::reference_wrapper<const asset_type>>
asset_manager::find_asset(asset_handle handle) const {
  return m_asset_storage.get_asset<asset_type>(handle);
}

template <typename asset_type>
[[nodiscard]] std::vector<
    std::reference_wrapper<const std::pair<asset_handle, asset_type>>>
asset_manager::find_assets() const {
  return m_asset_storage.find_assets_of<asset_type>();
}

}  // namespace wunder
#endif  // WUNDER_ASSET_MANAGER_H