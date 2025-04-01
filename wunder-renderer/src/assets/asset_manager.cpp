#include "assets/asset_manager.h"

#include <stb_image.h>
#include <tiny_gltf.h>

#include "assets/serializers/environment_map_serializer.h"
#include "assets/serializers/gltf/gltf_asset_importer.h"
#include "core/wunder_filesystem.h"

namespace wunder {

asset_manager::asset_manager()
    : m_gltf(std::make_unique<tinygltf::TinyGLTF>()),
      m_asset_importer(std::make_unique<gltf_asset_importer>(m_asset_storage)) {
  m_load_fns.emplace(
      ".gltf", std::bind(&tinygltf::TinyGLTF::LoadASCIIFromFile, m_gltf.get(),
                         std::placeholders::_1, std::placeholders::_2,
                         std::placeholders::_3, std::placeholders::_4,
                         std::placeholders::_5));
  m_load_fns.emplace(
      ".glb", std::bind(&tinygltf::TinyGLTF::LoadBinaryFromFile, m_gltf.get(),
                        std::placeholders::_1, std::placeholders::_2,
                        std::placeholders::_3, std::placeholders::_4,
                        std::placeholders::_5));
}
asset_manager::~asset_manager() = default;

asset_serialization_result_codes asset_manager::import_asset(
    const std::filesystem::path &asset) {
  auto scene_real_path = wunder_filesystem::instance().resolve_path(asset);
  AssertReturnUnless(std::filesystem::exists(scene_real_path),
                     asset_serialization_result_codes::error);

  AssertReturnUnless(scene_real_path.has_extension(),
                     asset_serialization_result_codes::error);

  std::string warn, error;
  tinygltf::Model gltf_model;

  auto asset_extension = scene_real_path.extension().string();
  auto load_fn = m_load_fns.find(asset_extension);
  ReturnIf(load_fn == m_load_fns.end(),
           asset_serialization_result_codes::not_supported_format_error);

  if (!load_fn->second(&gltf_model, &error, &warn, scene_real_path,
                       tinygltf::REQUIRE_VERSION)) {
    WUNDER_ERROR_TAG("Asset", error);
    return asset_serialization_result_codes::error;
  }

  if (!warn.empty()) {
    WUNDER_WARN_TAG("Asset", warn);
  }

  return load_gltf_file(gltf_model);
}

asset_serialization_result_codes asset_manager::import_environment_map(
    const std::filesystem::path &asset) {
  auto environment_map_real_path =
      wunder_filesystem::instance().resolve_path(asset);
  AssertReturnUnless(std::filesystem::exists(environment_map_real_path),
                     asset_serialization_result_codes::error);

  int32_t width{0};
  int32_t height{0};
  int32_t component{0};
  int32_t required_components = STBI_rgb_alpha;

  float *pixels = stbi_loadf(environment_map_real_path.c_str(), &width, &height,
                             &component, required_components);

  return environment_map_serializer::import_asset(
      {.m_width = width,
       .m_height = height,
       .m_components = required_components,
       .m_pixels_ptr = pixels},
      m_asset_storage);
}

asset_serialization_result_codes asset_manager::load_gltf_file(
    tinygltf::Model &gltf_model) {
  auto result = m_asset_importer->import_asset(gltf_model);
  AssertReturnUnless(result == asset_serialization_result_codes::ok, result);

  return result;
}

}  // namespace wunder