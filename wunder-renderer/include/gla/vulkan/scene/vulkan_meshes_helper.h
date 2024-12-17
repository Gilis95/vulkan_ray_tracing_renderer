#ifndef WUNDER_VULKAN_MESHES_HELPER_H
#define WUNDER_VULKAN_MESHES_HELPER_H

#include <unordered_set>

#include "assets/asset_types.h"
#include "assets/scene_node.h"
#include "core/vector_map.h"
#include "core/wunder_memory.h"

namespace wunder {
class material_asset;
class mesh_asset;
namespace vulkan {

class top_level_acceleration_structure;
class bottom_level_acceleration_structure_build_info;
class buffer;

struct vulkan_mesh_scene_node;
struct vulkan_mesh;

class meshes_helper {
 public:
  static vector_map<asset_handle, std::reference_wrapper<const mesh_asset>>
  extract_mesh_assets(
      std::vector<std::reference_wrapper<scene_node>> meshe_scene_nodes);

  [[nodiscard]] static asset_ids extract_mesh_ids(
      std::vector<ref<scene_node>>& mesh_entities);

  static void create_mesh_scene_nodes(
      assets<mesh_asset>& mesh_entities,
      const assets<material_asset>& materials,
      const std::vector<ref<scene_node>>& mesh_scene_nodes,
      std::vector<vulkan_mesh_scene_node>& out_mesh_nodes);

  static void prepare_blas_build_info(
      const assets<mesh_asset>& mesh_entities,
      const assets<material_asset>& materials,
      vector_map<asset_handle, shared_ptr<vulkan_mesh>>& out_mesh_instances,
      std::vector<bottom_level_acceleration_structure_build_info>&
          out_build_infos);
  static void build_blas(
      const std::vector<bottom_level_acceleration_structure_build_info>&
          build_infos,
      vector_map<asset_handle, shared_ptr<vulkan_mesh>>& mesh_instances);

  static void create_top_level_acceleration_structure(
      const std::vector<vulkan_mesh_scene_node>& mesh_nodes,
      top_level_acceleration_structure& out_acceleration_structure);

  [[nodiscard]] static unique_ptr<buffer> create_mesh_instances_buffer(
      const std::vector<vulkan_mesh_scene_node>& mesh_nodes);

 private:
};
}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_MESHES_HELPER_H
