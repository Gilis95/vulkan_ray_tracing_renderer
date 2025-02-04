#include "gla/vulkan/vulkan_device_buffer.h"

#include <cstring>

#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_memory_allocator.h"

namespace wunder::vulkan {

template <typename base_buffer_type>
device_buffer<base_buffer_type>::device_buffer(
    descriptor_build_data descriptor_build_data, size_t data_size,
    VkBufferUsageFlags usage_flags)
    : buffer<base_buffer_type>(std::move(descriptor_build_data)) {
  allocate_device_buffer(data_size, usage_flags);
}

template <typename base_buffer_type>
device_buffer<base_buffer_type>::device_buffer(
    descriptor_build_data descriptor_build_data, const void* data,
    size_t data_size, VkBufferUsageFlags usage_flags)
    : buffer<base_buffer_type>(std::move(descriptor_build_data)) {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.mutable_resource_allocator();
  auto& device = vulkan_context.mutable_device();

  auto [staging_buffer, staging_buffer_allocation] =
      allocate_cpu_staging_buffer(data_size);

  {  // Copy data to staging shader_group_buffer
    auto* dest_data = allocator.map_memory<uint8_t>(staging_buffer_allocation);
    std::memcpy(dest_data, data, data_size);
    allocator.unmap_memory(staging_buffer_allocation);
  }

  {  // allocate device memory
    allocate_device_buffer(data_size, usage_flags);
  }

  {  // copy host staging buffer to device
    auto& vulkan_command_pool = device.get_command_pool();
    VkCommandBuffer copyCmd =
        vulkan_command_pool.get_current_compute_command_buffer();

    VkBufferCopy copyRegion = {};
    copyRegion.size = data_size;
    vkCmdCopyBuffer(copyCmd, staging_buffer, buffer<base_buffer_type>::m_vk_buffer, 1, &copyRegion);

    vulkan_command_pool.flush_compute_command_buffer();
  }

  base_buffer_type::m_descriptor.buffer = buffer<base_buffer_type>::m_vk_buffer;
  base_buffer_type::m_descriptor.offset = 0;
  base_buffer_type::m_descriptor.range = VK_WHOLE_SIZE;

  // cleanup staging data
  allocator.destroy_buffer(staging_buffer, staging_buffer_allocation);
}

template <typename base_buffer_type>
device_buffer<base_buffer_type>::~device_buffer() = default;

template <typename base_buffer_type>
void device_buffer<base_buffer_type>::update_data(void* data, size_t data_size) /*override*/ {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();

  auto graphics_queue = device.get_command_pool().get_current_graphics_command_buffer();

  // Schedule the host-to-device upload. (hostUBO is copied into the cmd
  // buffer so it is okay to deallocate when the function returns).
  vkCmdUpdateBuffer(graphics_queue, buffer<base_buffer_type>::m_vk_buffer, 0, data_size, data);

  // Making sure the updated UBO will be visible.
  VkBufferMemoryBarrier afterBarrier{VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER};
  afterBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  afterBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  afterBarrier.buffer = buffer<base_buffer_type>::m_vk_buffer;
  afterBarrier.size = data_size;
  vkCmdPipelineBarrier(graphics_queue, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                           VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
                       VK_DEPENDENCY_DEVICE_GROUP_BIT, 0, nullptr, 1,
                       &afterBarrier, 0, nullptr);
}

template <typename base_buffer_type>
std::pair<VkBuffer, VmaAllocation> device_buffer<base_buffer_type>::allocate_cpu_staging_buffer(
    size_t data_size) {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.mutable_resource_allocator();

  VkBuffer staging_buffer;
  VmaAllocation staging_buffer_allocation;

  VkBufferCreateInfo buffer_create_info{};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = data_size;
  buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  staging_buffer_allocation = allocator.allocate_buffer(
      buffer_create_info, VMA_MEMORY_USAGE_CPU_TO_GPU, staging_buffer);

  return {staging_buffer, staging_buffer_allocation};
}

template <typename base_buffer_type>
void device_buffer<base_buffer_type>::allocate_device_buffer(size_t data_size,
                                           VkBufferUsageFlags usage_flags) {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.mutable_resource_allocator();

  VkBufferCreateInfo vertex_buffer_create_info = {};
  vertex_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vertex_buffer_create_info.size = data_size;
  vertex_buffer_create_info.usage =
      usage_flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  buffer<base_buffer_type>::m_allocation = allocator.allocate_buffer(
      vertex_buffer_create_info, VMA_MEMORY_USAGE_GPU_ONLY, buffer<base_buffer_type>::m_vk_buffer);
}


template class device_buffer<
    wunder::vulkan::shader_resource::instance::storage_buffers>;
template class device_buffer<
    wunder::vulkan::shader_resource::instance::uniform_buffer>;
}  // namespace wunder::vulkan