#include "gla/vulkan/vulkan_scene.h"

#include <vk_mem_alloc.h>

#include "assets/asset_manager.h"
#include "assets/scene_asset.h"
#include "core/project.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_bottom_level_acceleration_structure.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "resources/shaders/compress.glsl"
#include "resources/shaders/host_device.h"

namespace wunder {

[[nodiscard]] static vulkan_buffer create_device_buffer(
    const void* data, size_t data_size, VkBufferUsageFlags usage_flags);

[[nodiscard]] static vulkan_buffer create_vertex_buffer(const mesh_asset& mesh);
[[nodiscard]] static vulkan_buffer create_index_buffer(const mesh_asset& mesh);

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

  vulkan_bottom_level_acceleration_structure
      bottom_level_acceleration_structure;
  for (auto mesh_entity : mesh_entities) {
    std::optional<std::reference_wrapper<const mesh_component>>
        maybe_mesh_component =
            mesh_entity.get().get_component<mesh_component>();
    AssertContinueUnless(maybe_mesh_component.has_value(), VK_NULL_HANDLE);

    const mesh_component& mesh_cmp = maybe_mesh_component.value();

    auto maybe_mesh_asset =
        asset_manager.find_asset<mesh_asset>(mesh_cmp.m_handle);
    AssertContinueUnless(maybe_mesh_asset.has_value());

    const mesh_asset& mesh = maybe_mesh_asset.value();

    auto vertex_buffer = create_vertex_buffer(mesh);
    auto index_buffer = create_index_buffer(mesh);

    bottom_level_acceleration_structure.add_mesh_data(mesh, vertex_buffer,
                                                      index_buffer);
  }

  bottom_level_acceleration_structure.build();
  // TODO:: Create acceleration structure

  auto& main_camera = camera_entities[0];  // TODO:: handle multiple cameras
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

  vulkan_buffer result = create_device_buffer(
      vertecies.data(), vertecies.size() * sizeof(VertexAttributes),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);

  return result;
}

vulkan_buffer create_index_buffer(const mesh_asset& mesh) {
  vulkan_buffer result = create_device_buffer(
      mesh.m_indecies.data(), mesh.m_indecies.size() * sizeof(std::uint32_t),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);

  return result;
}

vulkan_buffer create_device_buffer(const void* data, size_t data_size,
                                   VkBufferUsageFlags usage_flags) {
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.get_resource_allocator();
  auto& device = vulkan_context.get_device();

  vulkan_buffer result{};

  VmaAllocation staging_buffer_allocation;
  VkBuffer staging_buffer;
  {  // allocate buffer on host side
    VkBufferCreateInfo buffer_create_info{};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = data_size;
    buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    staging_buffer_allocation = allocator.allocate_buffer(
        buffer_create_info, VMA_MEMORY_USAGE_CPU_TO_GPU, staging_buffer);
  }

  {  // Copy data to staging shader_group_buffer
    auto* dest_data = allocator.map_memory<uint8_t>(staging_buffer_allocation);
    memcpy(dest_data, data, data_size);
    allocator.unmap_memory(staging_buffer_allocation);
  }

  {  // allocate device memory
    VkBufferCreateInfo vertex_buffer_create_info = {};
    vertex_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertex_buffer_create_info.size = data_size;
    vertex_buffer_create_info.usage = usage_flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT ;

    result.m_allocation = allocator.allocate_buffer(vertex_buffer_create_info,
                                                    VMA_MEMORY_USAGE_GPU_ONLY,
                                                    result.m_vk_buffer);
  }

  {  // copy host staging buffer to device
    auto& vulkan_command_pool = device.get_command_pool();
    VkCommandBuffer copyCmd =
        vulkan_command_pool.get_current_compute_command_buffer();

    VkBufferCopy copyRegion = {};
    copyRegion.size = data_size;
    vkCmdCopyBuffer(copyCmd, staging_buffer, result.m_vk_buffer, 1,
                    &copyRegion);

    vulkan_command_pool.flush_compute_command_buffer();
  }

  // cleanup staging data
  allocator.destroy_buffer(staging_buffer, staging_buffer_allocation);

  return result;
}

}  // namespace wunder