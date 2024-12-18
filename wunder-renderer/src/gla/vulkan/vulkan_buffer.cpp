#include "gla/vulkan/vulkan_buffer.h"

#include <utility>

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "gla/vulkan/vulkan_renderer.h"

namespace wunder::vulkan {

buffer::buffer() : buffer({.m_enabled = false}) {}

buffer::buffer(descriptor_build_data descriptor_build_data)
    : m_descriptor_build_data(std::move(descriptor_build_data)) {}

buffer::buffer(buffer&& other) noexcept
    : m_descriptor_build_data(std::move(other.m_descriptor_build_data)),
      m_vk_buffer(other.m_vk_buffer),
      m_allocation(other.m_allocation) {
  other.m_vk_buffer = VK_NULL_HANDLE;
  other.m_allocation = VK_NULL_HANDLE;
}

buffer& buffer::operator=(buffer&& other) noexcept {
  std::swap(m_vk_buffer, other.m_vk_buffer);
  std::swap(m_allocation, other.m_allocation);

  return *this;
}

buffer::~buffer() {
  ReturnIf(m_vk_buffer == VK_NULL_HANDLE);
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.get_resource_allocator();
  allocator.destroy_buffer(m_vk_buffer, m_allocation);
}

void buffer::bind(renderer& renderer) /*override*/
{
  ReturnUnless(m_descriptor_build_data.m_enabled);

  auto& descriptor_manager = renderer.get_descriptor_set_manager();
  descriptor_manager.add_resource(m_descriptor_build_data.m_descriptor_name,
                                  *this);
}

[[nodiscard]] VkDeviceAddress buffer::get_address() const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  VkBufferDeviceAddressInfo info;
  std::memset(&info, 0, sizeof(VkBufferDeviceAddressInfo));

  info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  info.buffer = m_vk_buffer;

  return vkGetBufferDeviceAddress(device.get_vulkan_logical_device(), &info);
}

}  // namespace wunder::vulkan