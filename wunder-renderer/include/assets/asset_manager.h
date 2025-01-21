//
// Created by christian on 9/10/24.
//

#ifndef WUNDER_ASSET_MANAGER_H
#define WUNDER_ASSET_MANAGER_H

#include <filesystem>

#include "assets/asset_storage.h"
#include "assets/asset_types.h"
#include "core/vector_map.h"
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

  asset_serialization_result_codes import_environment_map(
      const std::filesystem::path& asset);

 public:
  template <typename asset_type>
  optional_const_ref<asset_type> find_asset(asset_handle handle) const;

  template <typename asset_type, typename input_iterator>
  assets<asset_type> find_assets(input_iterator begin,
                                 input_iterator end) const;

  template <typename asset_type>
  [[nodiscard]] assets<asset_type> find_assets() const;

 private:
  asset_serialization_result_codes load_gltf_file(tinygltf::Model& gltf_model);

 private:
  asset_storage m_asset_storage;
};

template <typename asset_type>
optional_const_ref<asset_type> asset_manager::find_asset(
    asset_handle handle) const {
  return m_asset_storage.find_asset<asset_type>(handle);
}

template <typename asset_type, typename input_iterator>
assets<asset_type> asset_manager::find_assets(input_iterator begin,
                                              input_iterator end) const {
  assets<asset_type> result;

  while (begin != end) {
    asset_handle handle = *begin;
    ++begin;

    ContinueUnless(handle.is_valid());

    auto maybe_asset = m_asset_storage.find_asset<asset_type>(handle);
    AssertContinueUnless(maybe_asset.has_value());

    const_ref<asset_type> asset = *maybe_asset;
    result.emplace_back(std::make_pair(handle, std::move(asset)));
  }

  return result;
}

template <typename asset_type>
[[nodiscard]] assets<asset_type> asset_manager::find_assets() const {
  return m_asset_storage.find_assets_of<asset_type>();
}

}  // namespace wunder
#endif  // WUNDER_ASSET_MANAGER_H
