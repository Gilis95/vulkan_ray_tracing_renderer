#include "scene/scene.h"

#include <glad/vulkan.h>

#include <filesystem>

#include "assets/asset_types.h"
#include "assets/gltf_scene_serializer.h"
#include "assets/scene_asset.h"
#include "core/wunder_filesystem.h"
#include "gla/vulkan/vulkan_scene.h"

namespace wunder {
scene_id scene::s_current_id = 0;

std::expected<scene_id, asset_serialization_result_codes> scene::load_scene(
    const std::filesystem::path& gltf_scene_path) {
  auto scene_real_path =
      wunder_filesystem::instance().resolve_path(gltf_scene_path);
  AssertReturnUnless(std::filesystem::exists(scene_real_path),
                     std::unexpected(std::unexpected(
                         asset_serialization_result_codes::error)));

  tinygltf::TinyGLTF gltf;

  std::string warn, error;
  tinygltf::Model gltf_model;

  AssertReturnUnless(
      gltf.LoadASCIIFromFile(&gltf_model, &error, &warn, scene_real_path),
      std::unexpected(asset_serialization_result_codes::error));

  return load_gltf_scene(gltf_model);
}

std::expected<scene_id, asset_serialization_result_codes>
scene::load_gltf_scene(tinygltf::Model& gltf_scene_root) {
  auto result = gltf_scene_serializer::serialize(gltf_scene_root);
  AssertReturnUnless(result.has_value(), std::unexpected(result.error()));

  m_loaded_scenes.emplace_back(
      std::make_pair(s_current_id, std::move(result.value())));
  return s_current_id++;
}

bool scene::activate_scene(scene_id id) {
  auto found_active_scene_it = m_active_scenes.find(id);
  ReturnIf(found_active_scene_it != m_active_scenes.end(), false);

  auto found_scene_asset_it = m_loaded_scenes.find(id);
  AssertReturnIf(found_scene_asset_it == m_loaded_scenes.end(), false);

  auto api_scene = vulkan_scene();
  api_scene.start_binding();
  found_scene_asset_it->second.iterate_nodes_components(api_scene);
  api_scene.finish_binding();

  m_active_scenes.emplace_back(std::make_pair(id, std::move(api_scene)));

  return true;
}

bool scene::deactivate_scene(scene_id id) {
  auto active_scene_it = m_active_scenes.find(id);
  ReturnIf(active_scene_it == m_active_scenes.end(), false);
  m_active_scenes.erase(active_scene_it);
}

}  // namespace wunder