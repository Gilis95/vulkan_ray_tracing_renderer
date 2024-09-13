#include "gla/vulkan/vulkan_buffer.h"

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_memory_allocator.h"

namespace wunder {

vulkan_buffer::vulkan_buffer(vulkan_buffer&& other) noexcept
    : m_vk_buffer(other.m_vk_buffer), m_allocation(other.m_allocation) {
  other.m_vk_buffer = VK_NULL_HANDLE;
  other.m_allocation = VK_NULL_HANDLE;
}

vulkan_buffer& vulkan_buffer::operator=(vulkan_buffer&& other) noexcept {
  std::swap(m_vk_buffer, other.m_vk_buffer);
  std::swap(m_allocation, other.m_allocation);

  return *this;
}

vulkan_buffer::~vulkan_buffer() {
  ReturnIf(m_vk_buffer == VK_NULL_HANDLE);
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.get_resource_allocator();
  allocator.destroy_buffer(m_vk_buffer, m_allocation);
}

[[nodiscard]] VkDeviceAddress vulkan_buffer::get_address() const {
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  VkBufferDeviceAddressInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  info.pNext = nullptr;
  info.buffer = m_vk_buffer;

  return vkGetBufferDeviceAddress(device.get_vulkan_logical_device(), &info);
}

}  // namespace wunder