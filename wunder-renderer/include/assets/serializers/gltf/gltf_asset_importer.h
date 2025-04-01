#ifndef WUNDER_GLTF_ASSET_IMPORTER_H
#define WUNDER_GLTF_ASSET_IMPORTER_H

#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "assets/asset_types.h"

// TODO:: This will include all possible components

#define KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME "KHR_lights_punctual"

namespace tinygltf {
class Model;
}

namespace wunder {
class scene_asset;
enum class asset_serialization_result_codes;
}  // namespace wunder

namespace wunder {
class asset_storage;
class gltf_asset_importer final {
 public:
  gltf_asset_importer(asset_storage& storage);

 public:
  asset_serialization_result_codes import_asset(tinygltf::Model& gltf_model);

 private:
  void check_required_extensions(
      const std::vector<std::string>& required_extensions);

  std::unordered_map<std::uint32_t, asset_handle> import_textures(
      const tinygltf::Model& gltf_scene_root);

  std::unordered_map<std::uint32_t, asset_handle> import_materials(
      const tinygltf::Model& gltf_scene_root,
      const std::unordered_map<std::uint32_t, asset_handle>& textures_map);

  std::unordered_map<std::uint32_t, asset_handle> import_lights(
      const tinygltf::Model& gltf_scene_root);

  std::unordered_map<std::uint32_t, asset_handle> import_cameras(
      tinygltf::Model& model);

  std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<asset_handle>>
  import_meshes(tinygltf::Model& gltf_scene_root,
                const std::unordered_map<uint32_t, asset_handle>& material_map);

  asset_serialization_result_codes import_scenes(
    const tinygltf::Model& gltf_root_node,
    std::unordered_map<std::uint32_t /*mesh_id*/, std::vector<asset_handle>>&
        mesh_id_to_primitive,
    std::unordered_map<std::uint32_t, asset_handle> cameras_map,
    std::unordered_map<std::uint32_t, asset_handle> lights_map);


 private:
  asset_storage& m_storage;
};
}  // namespace wunder
#endif  // WUNDER_GLTF_ASSET_IMPORTER_H