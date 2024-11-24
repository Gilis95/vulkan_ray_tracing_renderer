#include "include/gla/vulkan/scene/vulkan_scene.h"

#include <functional>

#include "assets/asset_manager.h"
#include "assets/scene_asset.h"
#include "core/project.h"
#include "core/vector_map.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/scene/vulkan_materials_helper.h"
#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/scene/vulkan_mesh_scene_node.h"
#include "gla/vulkan/scene/vulkan_meshes_helper.h"
#include "gla/vulkan/scene/vulkan_textures_helper.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "gla/vulkan/vulkan_texture.h"
#include "resources/shaders/host_device.h"

namespace wunder {

void create_material_buffer(
    wunder::vector_map<asset_handle,
                       std::reference_wrapper<const material_asset>>
        material_assets);

vulkan_scene::vulkan_scene() = default;
vulkan_scene::~vulkan_scene() = default;

vulkan_scene::vulkan_scene(vulkan_scene&&) = default;
vulkan_scene& vulkan_scene::operator=(vulkan_scene&&) noexcept = default;

void vulkan_scene::load_scene(scene_asset& asset) {
  auto mesh_entities =
      asset.filter_nodes<mesh_component, transform_component>();
  AssertReturnIf(mesh_entities.empty(), );  // nothing to render

  auto mesh_assets = vulkan_meshes_helper::extract_mesh_assets(mesh_entities);
  auto material_assets =
      vulkan_materials_helper::extract_material_assets(mesh_assets);
  auto texture_assets =
      vulkan_textures_helper::extract_texture_assets(material_assets);

  m_bound_textures =
      vulkan_textures_helper::create_texture_buffers(texture_assets);
//  create_material_buffer(material_assets);

  vulkan_meshes_helper::create_mesh_scene_nodes(mesh_assets, mesh_entities,
                                                m_mesh_nodes);
  AssertReturnIf(m_mesh_nodes.empty());

  m_acceleration_structure =
      std::make_unique<vulkan_top_level_acceleration_structure>();
  vulkan_meshes_helper::create_top_level_acceleration_structure(
      m_mesh_nodes, *m_acceleration_structure);
  //  auto& main_camera = camera_entities[0];  // TODO:: handle multiple cameras
}

void create_material_buffer(
    const wunder::vector_map<asset_handle, const_ref<material_asset>>&
        material_assets) {
  std::vector<GltfShadeMaterial> shader_materials;
  for (auto& [handle, material_ref] : material_assets) {
    auto& shader_material = shader_materials.emplace_back();
    shader_material = material_ref.get();
  }

  vulkan_device_buffer{shader_materials.data(),
                       shader_materials.size() * sizeof(GltfShadeMaterial),
                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                           VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT};
}

}  // namespace wunder