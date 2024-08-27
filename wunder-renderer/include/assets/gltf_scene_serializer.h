#ifndef WUNDER_GLTF_SCENE_SERIALIZER_H
#define WUNDER_GLTF_SCENE_SERIALIZER_H

#include <cstdint>
#include <expected>
#include <string>
#include <unordered_set>
#include <vector>

// TODO:: This will include all possible components
#include "assets/scene_node.h"
#include "tiny_gltf.h"

#define KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME "KHR_lights_punctual"

namespace tinygltf {
class Model;
}

namespace wunder {
class scene_asset;
enum class asset_serialization_result_codes;

class gltf_scene_serializer {
 public:
  static std::expected<scene_asset, asset_serialization_result_codes> serialize(
      tinygltf::Model& gltf_scene_root);

 private:
  static void find_used_meshes(const tinygltf::Model& tmodel,
                               std::unordered_set<uint32_t>& usedMeshes,
                               int nodeIdx);
  static void check_required_extensions(
      const std::vector<std::string>& required_extensions);

  static asset_serialization_result_codes process_nodes(
      const tinygltf::Model& gltf_root_node,
      const tinygltf::Scene& scene_nodes_to_process,
      std::unordered_map<std::uint32_t /*mesh_id*/,
                         std::vector<mesh_component>>& mesh_id_to_primitive,
      scene_asset& out_scene);

  static void process_materials(const tinygltf::Model& gltf_scene_root,
                                scene_asset& out_scene);

 private:
  static std::unordered_set<std::string> s_supported_extensions;
};
}  // namespace wunder
#endif  // WUNDER_GLTF_SCENE_SERIALIZER_H