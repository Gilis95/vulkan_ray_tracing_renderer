#ifndef WUNDER_VULKAN_MESHES_HELPER_H
#define WUNDER_VULKAN_MESHES_HELPER_H

#include <unordered_set>

#include "assets/asset_types.h"
#include "assets/scene_node.h"
#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"

namespace wunder {
class material_asset;
class mesh_asset;
namespace vulkan {

class top_level_acceleration_structure;
class bottom_level_acceleration_structure_build_info;

struct vulkan_mesh_scene_node;
struct vulkan_mesh;

class meshes_resource_creator {
 public:
  meshes_resource_creator(
      std::vector<std::reference_wrapper<scene_node>>& input_mesh_scene_nodes,
      std::vector<vulkan_mesh_scene_node>& out_vulkan_mesh_scene_nodes);

 public:
  [[nodiscard]] assets<mesh_asset>& extract_mesh_assets();

  void create_mesh_scene_nodes(const assets<material_asset>& materials);

  [[nodiscard]] unique_ptr<storage_buffer> create_mesh_instances_buffer();

 private:
  [[nodiscard]] asset_ids extract_mesh_ids( );

  void create_index_and_vertex_buffer(
      const assets<material_asset>& materials,
      vector_map<asset_handle, shared_ptr<vulkan_mesh>>& out_mesh_instances);

 private:
  std::vector<std::reference_wrapper<scene_node>>& m_input_mesh_scene_nodes;
  std::vector<vulkan_mesh_scene_node>& m_out_vulkan_mesh_nodes;

  assets<mesh_asset> m_input_mesh_assets;
};
}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_MESHES_HELPER_H
