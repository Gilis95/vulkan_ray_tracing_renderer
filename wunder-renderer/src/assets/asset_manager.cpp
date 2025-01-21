#include "assets/asset_manager.h"

#include <stb_image.h>
#include <tiny_gltf.h>

#include "assets/serializers/environment_map_serializer.h"
#include "assets/serializers/gltf/gltf_asset_importer.h"
#include "core/wunder_filesystem.h"

namespace wunder {

asset_manager::asset_manager() = default;
asset_manager::~asset_manager() = default;

asset_serialization_result_codes asset_manager::import_asset(
    const std::filesystem::path& asset) {
  auto scene_real_path = wunder_filesystem::instance().resolve_path(asset);
  AssertReturnUnless(std::filesystem::exists(scene_real_path),
                     asset_serialization_result_codes::error);

  tinygltf::TinyGLTF gltf;

  std::string warn, error;
  tinygltf::Model gltf_model;

  AssertReturnUnless(
      gltf.LoadASCIIFromFile(&gltf_model, &error, &warn, scene_real_path),
      asset_serialization_result_codes::error);

  return load_gltf_file(gltf_model);
}

asset_serialization_result_codes asset_manager::import_environment_map(
    const std::filesystem::path& asset) {
  auto environment_map_real_path =
      wunder_filesystem::instance().resolve_path(asset);
  AssertReturnUnless(std::filesystem::exists(environment_map_real_path),
                     asset_serialization_result_codes::error);

  int32_t width{0};
  int32_t height{0};
  int32_t component{0};
  int32_t required_components = STBI_rgb_alpha;

  float* pixels = stbi_loadf(environment_map_real_path.c_str(), &width, &height,
                             &component, required_components);

  return environment_map_serializer::import_asset({.m_width = width,
                                            .m_height = height,
                                            .m_components = required_components,
                                            .m_pixels_ptr = pixels},
                                           m_asset_storage);
}

asset_serialization_result_codes asset_manager::load_gltf_file(
    tinygltf::Model& gltf_model) {
  auto result = gltf_asset_importer::import_asset(gltf_model, m_asset_storage);
  AssertReturnUnless(result == asset_serialization_result_codes::ok, result);

  return result;
}

}  // namespace wunder