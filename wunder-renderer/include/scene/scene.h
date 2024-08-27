//
// Created by christian on 8/22/24.
//

#ifndef WUNDER_SCENE_H
#define WUNDER_SCENE_H

#include <expected>
#include <filesystem>

#include "core/vector_map.h"
#include "scene/scene_types.h"

namespace tinygltf {
class Model;
}

namespace wunder {

class scene_asset;
class vulkan_scene;
enum class asset_serialization_result_codes;

class scene {
 public:
  std::expected<scene_id, asset_serialization_result_codes> load_scene(
      const std::filesystem::path& gltf_scene);

  bool activate_scene(scene_id id);
  bool deactivate_scene(scene_id id);
 private:
      std::expected<scene_id, asset_serialization_result_codes> load_gltf_scene(
          tinygltf::Model& tmodel);
 private:
  vector_map<scene_id, scene_asset> m_loaded_scenes;
  vector_map<scene_id, vulkan_scene> m_active_scenes;

  static scene_id s_current_id;
};
}  // namespace wunder
#endif  // WUNDER_SCENE_H
