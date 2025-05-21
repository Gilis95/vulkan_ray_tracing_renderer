#include "gla/vulkan/scene/vulkan_scene.h"

#include <functional>
#include <ranges>

#include "assets/asset_manager.h"
#include "assets/scene_asset.h"
#include "core/project.h"
#include "core/vector_map.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"
#include "gla/vulkan/ray-trace/vulkan_rtx_renderer.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_builder.h"
#include "gla/vulkan/scene/vulkan_environment_resource_creator.h"
#include "gla/vulkan/scene/vulkan_lights_resource_creator.h"
#include "gla/vulkan/scene/vulkan_materials_resource_creator.h"
#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/scene/vulkan_mesh_scene_node.h"
#include "gla/vulkan/scene/vulkan_meshes_resource_creator.h"
#include "gla/vulkan/scene/vulkan_texture_resource_creator.h"
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

void scene::load_scene(const scene_asset& asset) {
  auto mesh_entities =
      asset.filter_nodes<mesh_component, transform_component>();
  AssertReturnIf(mesh_entities.empty(), );  // nothing to render

  auto light_entities =
      asset.filter_nodes<light_component, transform_component>();

  meshes_resource_creator _mesh_helper(mesh_entities, m_mesh_nodes);
  materials_resource_creator materials_resource_creator;
  texture_resource_creator texture_helper;

  auto& mesh_assets = _mesh_helper.extract_mesh_assets();
  auto& material_assets =
      materials_resource_creator.extract_material_assets(mesh_assets);

  m_bound_textures =
      std::move(texture_helper.create_texture_buffers(material_assets));
  m_material_buffer =
      std::move(materials_resource_creator.create_material_buffer(
          texture_helper.get_texture_assets()));
  m_light_buffer = std::move(
      lights_resource_creator::create_light_buffer(light_entities, m_lights_count));

  _mesh_helper.create_mesh_scene_nodes(material_assets);
  AssertReturnIf(m_mesh_nodes.empty());
  m_mesh_instance_data_buffer = _mesh_helper.create_mesh_instances_buffer();

  m_acceleration_structure =
      std::make_unique<top_level_acceleration_structure>();
  top_level_acceleration_structure_builder
      top_level_acceleration_structure_builder(
          *m_acceleration_structure, m_acceleration_structure_build_info,
          m_mesh_nodes);
  top_level_acceleration_structure_builder.build();

  m_sun_and_sky_properties_buffer =
      vulkan_environment_resource_creator::create_sky_and_sun_properties();
  m_environment_textures =
      std::move(vulkan_environment_resource_creator::create_environment_texture());
}

void scene::collect_descriptors(descriptor_set_manager& target) {
  for (auto& texture : m_bound_textures) {
    texture->add_descriptor_to(target);
  }

  m_material_buffer->add_descriptor_to(target);
  m_light_buffer->add_descriptor_to(target);
  m_mesh_instance_data_buffer->add_descriptor_to(target);
  m_acceleration_structure->add_descriptor_to(target);
  m_sun_and_sky_properties_buffer->add_descriptor_to(target);
  m_environment_textures.add_descriptor_to(target);
}

}  // namespace wunder::vulkan