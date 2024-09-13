#include "include/gla/vulkan/scene/vulkan_scene.h"

#include <functional>
#include <numeric>
#include <unordered_set>

#include "assets/asset_manager.h"
#include "assets/scene_asset.h"
#include "core/project.h"
#include "core/vector_map.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/scene/vulkan_mesh_scene_node.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "resources/shaders/compress.glsl"
#include "resources/shaders/host_device.h"

namespace wunder {

static void create_mesh_scene_nodes(
    std::vector<std::reference_wrapper<scene_node>>& mesh_entities,
    std::vector<vulkan_mesh_scene_node>& out_mesh_nodes);

void create_top_level_acceleration_structure(
    const std::vector<vulkan_mesh_scene_node>& mesh_nodes,
    vulkan_top_level_acceleration_structure& out_acceleration_structure);

[[nodiscard]] static std::unordered_set<asset_handle> extract_mesh_ids(
    std::vector<std::reference_wrapper<scene_node>>& mesh_entities);
static void prepare_blas_build_info(
    std::unordered_set<asset_handle>& mesh_ids,
    vector_map<asset_handle, shared_ptr<vulkan_mesh>>& out_mesh_instances,
    std::vector<vulkan_bottom_level_acceleration_structure_build_info>&
        out_build_infos);
static void build_blas(
    const std::vector<vulkan_bottom_level_acceleration_structure_build_info>&
        build_infos,
    vector_map<asset_handle, shared_ptr<vulkan_mesh>>& mesh_instances);

[[nodiscard]] static vulkan_buffer create_vertex_buffer(const mesh_asset& mesh);
[[nodiscard]] static vulkan_buffer create_index_buffer(const mesh_asset& mesh);

vulkan_scene::vulkan_scene() = default;
vulkan_scene::~vulkan_scene() = default;


vulkan_scene::vulkan_scene(vulkan_scene&&) = default;
vulkan_scene& vulkan_scene::operator=(vulkan_scene&&) noexcept = default;

void vulkan_scene::load_scene(scene_asset& asset) {
  auto mesh_entities =
      asset.filter_nodes<mesh_component, transform_component>();
  AssertReturnIf(mesh_entities.empty(), );  // nothing to render

  auto camera_entities = asset.filter_nodes<camera_component>();
  AssertReturnIf(camera_entities.empty(), );  // TODO:: Add default camera

  auto light_entities =
      asset.filter_nodes<light_component, transform_component>();
  AssertReturnIf(camera_entities.empty(), );  // TODO:: Add default camera

  auto& asset_manager = project::instance().get_asset_manager();

  auto texture_entities = asset.filter_nodes<texture_component>();
  for (auto& texture_entity : texture_entities) {
    // TODO:: create texture buffer
  }

  auto material_entities = asset.filter_nodes<material_component>();
  for (auto& material_entity : material_entities) {
    // TODO:: create material buffer
  }

  vector_map<asset_handle, shared_ptr<vulkan_mesh>> mesh_instances;
  create_mesh_scene_nodes(mesh_entities, m_mesh_nodes);

  AssertReturnIf(m_mesh_nodes.empty());

  m_acceleration_structure =
      std::make_unique<vulkan_top_level_acceleration_structure>();
  create_top_level_acceleration_structure(m_mesh_nodes,
                                          *m_acceleration_structure);
//  auto& main_camera = camera_entities[0];  // TODO:: handle multiple cameras
}

void create_mesh_scene_nodes(
    std::vector<std::reference_wrapper<scene_node>>& mesh_entities,
    std::vector<vulkan_mesh_scene_node>& out_mesh_nodes) {
  std::unordered_set<asset_handle> mesh_ids = extract_mesh_ids(mesh_entities);

  std::vector<vulkan_bottom_level_acceleration_structure_build_info>
      build_infos;

  // we first go through unique meshes and create them an instance
  vector_map<asset_handle, shared_ptr<vulkan_mesh>> mesh_instances;
  prepare_blas_build_info(mesh_ids, mesh_instances, build_infos);
  build_blas(build_infos, mesh_instances);

  // then we use the instances to create a scene nodes, placed in specific
  // world space
  for (auto& mesh_entity : mesh_entities) {
    auto maybe_transform_component =
        mesh_entity.get().get_component<transform_component>();
    AssertContinueUnless(maybe_transform_component.has_value());

    auto maybe_mesh_component =
        mesh_entity.get().get_component<mesh_component>();
    AssertContinueUnless(maybe_mesh_component.has_value());

    auto mesh_instance_it =
        mesh_instances.find(maybe_mesh_component->get().m_handle);
    AssertContinueIf(mesh_instance_it == mesh_instances.end());

    out_mesh_nodes.emplace_back(vulkan_mesh_scene_node{
        .m_mesh = mesh_instance_it->second,
        .m_model_matrix = maybe_transform_component->get().m_world_matrix});
  }
}

void create_top_level_acceleration_structure(
    const std::vector<vulkan_mesh_scene_node>& mesh_nodes,
    vulkan_top_level_acceleration_structure& out_acceleration_structure) {
  vulkan_top_level_acceleration_structure_build_info tlas_build_info(
      mesh_nodes);
  vulkan_buffer scratch_buffer =
      vulkan_device_buffer(tlas_build_info.get_vulkan_as_build_sizes_info()
                               .buildScratchSize,
                           VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

  out_acceleration_structure.build(scratch_buffer, tlas_build_info);
}

void prepare_blas_build_info(
    std::unordered_set<asset_handle>& mesh_ids,
    vector_map<asset_handle, shared_ptr<vulkan_mesh>>& out_mesh_instances,
    std::vector<vulkan_bottom_level_acceleration_structure_build_info>&
        out_build_infos) {
  auto& asset_manager = project::instance().get_asset_manager();

  std::uint32_t i = 0;
  for (auto mesh_id : mesh_ids) {
    auto maybe_mesh_asset = asset_manager.find_asset<mesh_asset>(mesh_id);
    AssertContinueUnless(maybe_mesh_asset.has_value());

    auto& [id, _vulkan_mesh] = out_mesh_instances.emplace_back();
    const mesh_asset& mesh_asset = maybe_mesh_asset.value();

    _vulkan_mesh = make_shared<vulkan_mesh>();
    _vulkan_mesh->m_vertex_buffer = std::move(create_vertex_buffer(mesh_asset));
    _vulkan_mesh->m_index_buffer = std::move(create_index_buffer(mesh_asset));
    _vulkan_mesh->idx = i;

    vulkan_bottom_level_acceleration_structure_build_info build_info(
        mesh_asset, _vulkan_mesh->m_vertex_buffer,
        _vulkan_mesh->m_index_buffer);
    out_build_infos.emplace_back(std::move(build_info));
    ++i;
  }
}

void build_blas(
    const std::vector<vulkan_bottom_level_acceleration_structure_build_info>&
        build_infos,
    vector_map<asset_handle, shared_ptr<vulkan_mesh>>& mesh_instances) {
  std::uint32_t scratch_buffer_size = 0;
  scratch_buffer_size = std::accumulate(
      build_infos.begin(), build_infos.end(), scratch_buffer_size,
      [](std::uint32_t current_accumulation,
         const vulkan_bottom_level_acceleration_structure_build_info& right) {
        return current_accumulation +
               right.get_vulkan_as_build_sizes_info().accelerationStructureSize;
      });

  vulkan_buffer scratch_buffer = vulkan_device_buffer(
      scratch_buffer_size, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                               VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

  std::uint32_t scratch_buffer_offset = 0;
  for (auto& [id, mesh_instance_ptr] : mesh_instances) {
    AssertContinueUnless(mesh_instance_ptr);
    vulkan_mesh& mesh_instance = *mesh_instance_ptr;

    AssertContinueUnless(build_infos.size() > mesh_instance.idx);
    auto& build_info = build_infos[mesh_instance.idx];

    mesh_instance.m_blas.build(scratch_buffer, scratch_buffer_offset,
                               build_info);
    scratch_buffer_offset +=
        build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize;
  }
}

std::unordered_set<asset_handle> extract_mesh_ids(
    std::vector<std::reference_wrapper<scene_node>>& mesh_entities) {
  std::unordered_set<asset_handle> mesh_ids;
  for (auto& mesh_entity : mesh_entities) {
    std::optional<std::reference_wrapper<const mesh_component>>
        maybe_mesh_component =
            mesh_entity.get().get_component<mesh_component>();
    AssertContinueUnless(maybe_mesh_component.has_value());

    mesh_ids.emplace(maybe_mesh_component->get().m_handle);
  }

  return mesh_ids;
}

vulkan_buffer create_vertex_buffer(const mesh_asset& mesh) {
  std::vector<VertexAttributes> vertecies{};
  for (auto& vertex : mesh.m_verticies) {
    VertexAttributes device_vertex{};
    device_vertex.position = vertex.m_position;
    device_vertex.normal = compress_unit_vec(vertex.m_normal);
    device_vertex.tangent =
        compress_unit_vec(vertex.m_tangent);  // See .w encoding below
    device_vertex.texcoord = vertex.m_texcoord;
    device_vertex.color = glm::packUnorm4x8(vertex.m_color);

    // Encode to the Less-Significant-Bit the handiness of the tangent
    // Not a significant change on the UV to make a visual difference
    // auto     uintBitsToFloat = [](uint32_t a) -> float { return
    // *(float*)&(a); }; auto     floatBitsToUint = [](float a) -> uint32_t {
    // return *(uint32_t*)&(a); };
    uint32_t value = floatBitsToUint(device_vertex.texcoord.y);
    if (vertex.m_tangent.w > 0) {
      value |= 1;  // set bit, H == +1
    } else {
      value &= ~1;  // clear bit, H == -1
    }
    device_vertex.texcoord.y = uintBitsToFloat(value);

    vertecies.push_back(std::move(device_vertex));
  }

  return vulkan_device_buffer{
      vertecies.data(), vertecies.size() * sizeof(VertexAttributes),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR};
}

vulkan_buffer create_index_buffer(const mesh_asset& mesh) {
  return vulkan_device_buffer{
      mesh.m_indecies.data(), mesh.m_indecies.size() * sizeof(std::uint32_t),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR};
}

}  // namespace wunder