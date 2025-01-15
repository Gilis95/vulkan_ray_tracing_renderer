#include "gla/vulkan/scene/vulkan_meshes_helper.h"

#include <functional>
#include <numeric>
#include <unordered_set>

#include "assets/asset_manager.h"
#include "core/project.h"
#include "core/vector_map.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/scene/vulkan_mesh_scene_node.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_index_buffer.h"
#include "gla/vulkan/vulkan_vertex_buffer.h"

namespace wunder::vulkan {

vector_map<asset_handle, const_ref<mesh_asset>>
meshes_helper::extract_mesh_assets(
    std::vector<ref<scene_node>> meshe_scene_nodes) {
  auto& asset_manager = project::instance().get_asset_manager();

  std::unordered_set<asset_handle> mesh_ids =
      meshes_helper::extract_mesh_ids(meshe_scene_nodes);

  vector_map<asset_handle, const_ref<mesh_asset>> result =
      asset_manager.find_assets<mesh_asset>(mesh_ids.begin(), mesh_ids.end());

  return result;
}

asset_ids meshes_helper::extract_mesh_ids(
    std::vector<ref<scene_node>>& mesh_entities) {
  std::unordered_set<asset_handle> mesh_ids;
  for (auto& mesh_entity : mesh_entities) {
    optional_const_ref<mesh_component> maybe_mesh_component =
        mesh_entity.get().get_component<mesh_component>();
    AssertContinueUnless(maybe_mesh_component.has_value());

    mesh_ids.emplace(maybe_mesh_component->get().m_handle);
  }

  return mesh_ids;
}

void meshes_helper::create_mesh_scene_nodes(
    assets<mesh_asset>& mesh_entities, const assets<material_asset>& materials,
    const std::vector<ref<scene_node>>& mesh_scene_nodes,
    std::vector<vulkan_mesh_scene_node>& out_mesh_nodes) {
  std::vector<bottom_level_acceleration_structure_build_info> build_infos;

  // we first go through unique meshes and create them an instance
  vector_map<asset_handle, shared_ptr<vulkan_mesh>> mesh_instances;
  prepare_blas_build_info(mesh_entities, materials, mesh_instances,
                          build_infos);
  build_blas(build_infos, mesh_instances);

  // then we use the instances to create a scene nodes, placed in specific
  // world space
  for (const auto& mesh_entity : mesh_scene_nodes) {
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

    out_mesh_nodes.emplace_back(vulkan_mesh_scene_node{
        .m_mesh = mesh_instance,
        .m_model_matrix = maybe_transform_component->get().m_world_matrix,
        });
  }
}

void meshes_helper::prepare_blas_build_info(
    const assets<mesh_asset>& mesh_entities,
    const assets<material_asset>& materials,
    vector_map<asset_handle, shared_ptr<vulkan_mesh>>& out_mesh_instances,
    std::vector<bottom_level_acceleration_structure_build_info>&
        out_build_infos) {
  std::uint32_t i = 0;
  for (const auto& [mesh_id, mesh_asset] : mesh_entities) {
    auto& [id, _vulkan_mesh] = out_mesh_instances.emplace_back();

    auto material_it = materials.find(mesh_asset.get().m_material_handle);
    long material_idx = material_it == materials.end()
                            ? 0
                            : std::distance(materials.begin(), material_it);

    _vulkan_mesh = make_shared<vulkan_mesh>();
    _vulkan_mesh->m_vertex_buffer =
        std::move(vertex_buffer::create(mesh_asset));
    _vulkan_mesh->m_index_buffer = std::move(index_buffer::create(mesh_asset));
    _vulkan_mesh->m_idx = i;
    _vulkan_mesh->m_material_idx = material_idx;
    id = mesh_id;

    bottom_level_acceleration_structure_build_info build_info(
        mesh_asset, *_vulkan_mesh->m_vertex_buffer,
        *_vulkan_mesh->m_index_buffer);
    out_build_infos.emplace_back(std::move(build_info));
    ++i;
  }
}

void meshes_helper::build_blas(
    const std::vector<bottom_level_acceleration_structure_build_info>&
        build_infos,
    vector_map<asset_handle, shared_ptr<vulkan_mesh>>& mesh_instances) {
  std::uint32_t scratch_buffer_size = 0;
  scratch_buffer_size = std::accumulate(
      build_infos.begin(), build_infos.end(), scratch_buffer_size,
      [](std::uint32_t current_accumulation,
         const bottom_level_acceleration_structure_build_info& right) {
        return current_accumulation +
               right.get_vulkan_as_build_sizes_info().accelerationStructureSize;
      });

  buffer scratch_buffer = storage_device_buffer(
      descriptor_build_data{.m_enabled = false}, scratch_buffer_size,
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

  std::uint32_t scratch_buffer_offset = 0;
  for (auto& [id, mesh_instance_ptr] : mesh_instances) {
    AssertContinueUnless(mesh_instance_ptr);
    vulkan_mesh& mesh_instance = *mesh_instance_ptr;

    AssertContinueUnless(build_infos.size() > mesh_instance.m_idx);
    auto& build_info = build_infos[mesh_instance.m_idx];

    mesh_instance.m_blas.build(scratch_buffer, scratch_buffer_offset,
                               build_info);
    scratch_buffer_offset +=
        build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize;
  }
}

void meshes_helper::create_top_level_acceleration_structure(
    const std::vector<vulkan_mesh_scene_node>& mesh_nodes,
    top_level_acceleration_structure& out_acceleration_structure) {
  top_level_acceleration_structure_build_info tlas_build_info(mesh_nodes);
  buffer scratch_buffer = storage_device_buffer(
      descriptor_build_data{.m_enabled = false},
      tlas_build_info.get_vulkan_as_build_sizes_info().buildScratchSize,
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

  out_acceleration_structure.build(scratch_buffer, tlas_build_info);
}

unique_ptr<storage_buffer> meshes_helper::create_mesh_instances_buffer(
    const std::vector<vulkan_mesh_scene_node>& mesh_nodes) {
  std::vector<InstanceData> instances;

  for (const auto& mesh_node : mesh_nodes) {
    AssertContinueUnless(mesh_node.m_mesh);
    vulkan_mesh& mesh = *mesh_node.m_mesh;

    instances.emplace_back(InstanceData{
        .vertexAddress = mesh.m_idx,
        .indexAddress = mesh.m_idx,
        .materialIndex = static_cast<int>(
            mesh.m_material_idx)  // most probably will never overflow
    });
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

}  // namespace wunder::vulkan