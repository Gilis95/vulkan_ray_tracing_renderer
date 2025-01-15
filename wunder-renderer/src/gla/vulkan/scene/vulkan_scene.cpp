#include "gla/vulkan/scene/vulkan_scene.h"

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

namespace wunder::vulkan {
scene::scene() = default;
scene::~scene() = default;

scene::scene(scene&&) = default;
scene& scene::operator=(scene&&) noexcept = default;

void scene::load_scene(scene_asset& asset) {
  auto mesh_entities =
      asset.filter_nodes<mesh_component, transform_component>();
  AssertReturnIf(mesh_entities.empty(), );  // nothing to render

  auto mesh_assets = meshes_helper::extract_mesh_assets(mesh_entities);
  auto material_assets = materials_helper::extract_material_assets(mesh_assets);
  auto texture_assets =
      textures_helper::extract_texture_assets(material_assets);

  m_bound_textures = textures_helper::create_texture_buffers(texture_assets);
  m_material_buffer =
      materials_helper::create_material_buffer(material_assets, texture_assets);

  meshes_helper::create_mesh_scene_nodes(mesh_assets, material_assets,
                                         mesh_entities, m_mesh_nodes);
  AssertReturnIf(m_mesh_nodes.empty());
  m_mesh_instance_data_buffer =
      meshes_helper::create_mesh_instances_buffer(m_mesh_nodes);

  m_acceleration_structure =
      std::make_unique<top_level_acceleration_structure>();
  meshes_helper::create_top_level_acceleration_structure(
      m_mesh_nodes, *m_acceleration_structure);
  //  auto& main_camera = camera_entities[0];  // TODO:: handle multiple cameras
}

void scene::bind(renderer& renderer) {
  for (auto& texture : m_bound_textures) {
    texture->add_descriptor_to(renderer);
  }

  m_material_buffer->add_descriptor_to(renderer);
  m_mesh_instance_data_buffer->add_descriptor_to(renderer);
  m_acceleration_structure->add_descriptor_to(renderer);
}

}  // namespace wunder::vulkan