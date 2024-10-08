#include "gla/vulkan/vulkan_device_buffer.h"

#include <cstring>

#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_memory_allocator.h"

namespace wunder {

vulkan_device_buffer::vulkan_device_buffer(size_t data_size,
                                           VkBufferUsageFlags usage_flags) {
  allocate_device_buffer(data_size, usage_flags);
}

vulkan_device_buffer::vulkan_device_buffer(const void* data, size_t data_size,
                                           VkBufferUsageFlags usage_flags) {
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.get_resource_allocator();
  auto& device = vulkan_context.get_device();

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
    vkCmdCopyBuffer(copyCmd, staging_buffer, m_vk_buffer, 1, &copyRegion);

    vulkan_command_pool.flush_compute_command_buffer();
  }

  // cleanup staging data
  allocator.destroy_buffer(staging_buffer, staging_buffer_allocation);
}

std::pair<VkBuffer, VmaAllocation>
vulkan_device_buffer::allocate_cpu_staging_buffer(size_t data_size) {
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.get_resource_allocator();

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

void vulkan_device_buffer::allocate_device_buffer(
    size_t data_size, VkBufferUsageFlags usage_flags) {
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.get_resource_allocator();

  VkBufferCreateInfo vertex_buffer_create_info = {};
  vertex_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vertex_buffer_create_info.size = data_size;
  vertex_buffer_create_info.usage =
      usage_flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

  m_allocation = allocator.allocate_buffer(
      vertex_buffer_create_info, VMA_MEMORY_USAGE_GPU_ONLY, m_vk_buffer);
}

vulkan_device_buffer::~vulkan_device_buffer() = default;

}  // namespace wunder