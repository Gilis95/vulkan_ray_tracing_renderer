#include "assets/asset_importer_task.h"

#include <bits/fs_path.h>
#include <tiny_gltf.h>

#include "assets/asset_types.h"
#include "assets/serializers/gltf/gltf_asset_importer.h"
#include "core/wunder_logger.h"

namespace wunder {
asset_importer_task::asset_importer_task(tinygltf::TinyGLTF& gltf,
                                         gltf_asset_importer& asset_importer,
                                         std::filesystem::path asset_path)
    : m_gltf(gltf), m_asset_importer(asset_importer), m_asset_path(asset_path) {
  m_load_fns.emplace(
      ".gltf", std::bind(&tinygltf::TinyGLTF::LoadASCIIFromFile, &m_gltf,
                         std::placeholders::_1, std::placeholders::_2,
                         std::placeholders::_3, std::placeholders::_4,
                         std::placeholders::_5));
  m_load_fns.emplace(
      ".glb", std::bind(&tinygltf::TinyGLTF::LoadBinaryFromFile, &m_gltf,
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4,
                        std::placeholders::_5));
}

bool asset_importer_task::is_file_supported() const {
  return m_load_fns.contains(m_asset_path.extension());
}

void asset_importer_task::run() /*override*/ {
  std::string warn, error;

  auto asset_extension = m_asset_path.extension().string();
  auto load_fn = m_load_fns.find(asset_extension);
  if (load_fn == m_load_fns.end()) {
    m_result_code =
        asset_serialization_result_codes::not_supported_format_error;
  }

  if (!load_fn->second(&gltf_model, &error, &warn, m_asset_path,
                       tinygltf::REQUIRE_VERSION)) {
    WUNDER_ERROR_TAG("Asset", error);
    m_result_code = asset_serialization_result_codes::error;
  }

  if (!warn.empty()) {
    WUNDER_WARN_TAG("Asset", warn);
  }

  m_result_code =
        asset_serialization_result_codes::ok;
}

void asset_importer_task::execute_on_main_thread() /*override*/ {
  ReturnIf(m_result_code != asset_serialization_result_codes::ok);

  auto result = m_asset_importer.import_asset(gltf_model);
  AssertReturnUnless(result == asset_serialization_result_codes::ok);
}

}  // namespace wunder