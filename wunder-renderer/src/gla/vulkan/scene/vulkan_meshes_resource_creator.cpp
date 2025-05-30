#include "gla/vulkan/scene/vulkan_meshes_resource_creator.h"

#include <functional>
#include <numeric>
#include <set>
#include <unordered_set>

#include "assets/asset_manager.h"
#include "core/project.h"
#include "core/vector_map.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_builder.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/scene/vulkan_mesh_scene_node.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_index_buffer.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_vertex_buffer.h"

namespace wunder::vulkan {
meshes_resource_creator::meshes_resource_creator(
    std::vector<const_ref<scene_node>>& input_mesh_scene_nodes,
    std::vector<vulkan_mesh_scene_node>& out_vulkan_mesh_scene_nodes)
    : m_input_mesh_scene_nodes(input_mesh_scene_nodes),
      m_out_vulkan_mesh_nodes(out_vulkan_mesh_scene_nodes) {}

vector_map<asset_handle, const_ref<mesh_asset>>&
meshes_resource_creator::extract_mesh_assets() {
  auto& asset_manager = project::instance().get_asset_manager();

  asset_ids mesh_ids = extract_mesh_ids();

  m_input_mesh_assets =
      asset_manager.find_assets<mesh_asset>(mesh_ids.begin(), mesh_ids.end());

  return m_input_mesh_assets;
}

void meshes_resource_creator::create_mesh_scene_nodes(
    const assets<material_asset>& materials) {
  // we first go through unique meshes and create them an instance
  vector_map<asset_handle, shared_ptr<vulkan_mesh>> mesh_instances;
  create_index_and_vertex_buffer(materials, mesh_instances);

  // then we use the instances to create a scene nodes, placed in specific
  // world space
  m_out_vulkan_mesh_nodes.reserve(m_input_mesh_scene_nodes.size());
  for (const auto& mesh_entity : m_input_mesh_scene_nodes) {
    auto maybe_transform_component =
        mesh_entity.get().get_component<transform_component>();
    AssertContinueUnless(maybe_transform_component.has_value());

    const auto maybe_mesh_component =
        mesh_entity.get().get_component<mesh_component>();
    AssertContinueUnless(maybe_mesh_component.has_value());

    auto mesh_instance_it =
        mesh_instances.find(maybe_mesh_component->get().m_handle);
    AssertContinueIf(mesh_instance_it == mesh_instances.end());

    auto& mesh_instance = mesh_instance_it->second;
    AssertContinueUnless(mesh_instance);

    m_out_vulkan_mesh_nodes.emplace_back(vulkan_mesh_scene_node{
        .m_mesh = mesh_instance,
        .m_model_matrix = maybe_transform_component->get().m_world_matrix,
    });
  }

  bottom_level_acceleration_structure_builder builder(m_out_vulkan_mesh_nodes);
  builder.build();
}

unique_ptr<storage_buffer>
meshes_resource_creator::create_mesh_instances_buffer() {
  std::vector<InstanceData> instances;
  instances.reserve(m_out_vulkan_mesh_nodes.size());

  for (const auto& mesh_node : m_out_vulkan_mesh_nodes) {
    AssertContinueUnless(mesh_node.m_mesh);
    vulkan_mesh& mesh = *mesh_node.m_mesh;

    instances.emplace_back(InstanceData{
        .vertexAddress = mesh.m_vertex_buffer->get_address(),
        .indexAddress = mesh.m_index_buffer->get_address(),
        .materialIndex = static_cast<int>(
            mesh.m_material_idx),  // most probably will never overflow
        ._pad = glm::vec3{0.f}});
  }

  unique_ptr<storage_buffer> result;
  ReturnIf(instances.empty(), result);

  return std::make_unique<storage_device_buffer>(
      descriptor_build_data{.m_enabled = true,
                            .m_descriptor_name = "_InstanceInfo"},
      instances.data(), instances.size() * sizeof(InstanceData),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);
}

asset_ids meshes_resource_creator::extract_mesh_ids() {
  asset_ids mesh_ids;

  for (auto& mesh_entity : m_input_mesh_scene_nodes) {
    optional_const_ref<mesh_component> maybe_mesh_component =
        mesh_entity.get().get_component<mesh_component>();
    AssertContinueUnless(maybe_mesh_component.has_value());

    auto handle = maybe_mesh_component->get().m_handle;

    mesh_ids.emplace(handle);
  }

  return mesh_ids;
}

void meshes_resource_creator::create_index_and_vertex_buffer(
    const assets<material_asset>& materials,
    vector_map<asset_handle, shared_ptr<vulkan_mesh>>& out_mesh_instances) {
  std::uint32_t i = 0;
  out_mesh_instances.reserve(m_input_mesh_assets.size());

  auto& command_pool = layer_abstraction_factory::instance()
                           .get_vulkan_context()
                           .mutable_device()
                           .get_command_pool();
  auto command_buffer = command_pool.get_current_graphics_command_buffer();

  for (const auto& [mesh_id, mesh_asset_ref] : m_input_mesh_assets) {
    auto& [id, _vulkan_mesh] = out_mesh_instances.emplace_back();

    auto mesh_asset = mesh_asset_ref.get();
    auto material_it = materials.find(mesh_asset.m_material_handle);
    std::uint32_t material_idx = material_it == materials.end()
                                     ? 0u
                                     : static_cast<uint32_t>(std::distance(
                                           materials.begin(), material_it));
    auto& material = material_it->second.get();

    _vulkan_mesh = make_shared<vulkan_mesh>();
    _vulkan_mesh->m_vertex_buffer =
        std::move(vertex_buffer::create(command_buffer, mesh_asset));
    _vulkan_mesh->m_vertices_count =
        static_cast<uint32_t>(mesh_asset.m_vertices.size());
    _vulkan_mesh->m_index_buffer =
        std::move(index_buffer::create(command_buffer, mesh_asset));
    _vulkan_mesh->m_indices_count =
        static_cast<uint32_t>(mesh_asset.m_indices.size());
    _vulkan_mesh->m_idx = i;
    _vulkan_mesh->m_material_idx = material_idx;
    _vulkan_mesh->m_is_opaque = material.m_alpha_mode == 0 ||
                                (material.m_pbr_base_color_factor.w == 1.0f &&
                                 !material.m_pbr_base_color_texture.is_valid());
    _vulkan_mesh->m_is_double_sided = material.m_double_sided;

    id = mesh_id;
    ++i;
  }

  command_pool.flush_graphics_command_buffer();

  // free staging data
  for (auto& [_, mesh] : out_mesh_instances) {
    AssertContinueUnless(mesh);

    if (mesh->m_vertex_buffer) {
      mesh->m_vertex_buffer->free_staging_data();
    }
    if (mesh->m_index_buffer) {
      mesh->m_index_buffer->free_staging_data();
    }
  }
}
}  // namespace wunder::vulkan