#include "gla/vulkan/vulkan_buffer.h"

#include <utility>

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "include/gla/vulkan/ray-trace/vulkan_rtx_renderer.h"

namespace wunder::vulkan {

template <typename base_buffer_type>
buffer<base_buffer_type>::buffer(descriptor_build_data descriptor_build_data)
    : m_descriptor_build_data(std::move(descriptor_build_data)) {}

template <typename base_buffer_type>
buffer<base_buffer_type>::buffer(buffer&& other) noexcept
    : m_descriptor_build_data(std::move(other.m_descriptor_build_data)),
      m_vk_buffer(other.m_vk_buffer),
      m_allocation(other.m_allocation) {
  other.m_vk_buffer = VK_NULL_HANDLE;
  other.m_allocation = VK_NULL_HANDLE;
}

template <typename base_buffer_type>
buffer<base_buffer_type>& buffer<base_buffer_type>::operator=(
    buffer<base_buffer_type>&& other) noexcept {
  std::swap(m_vk_buffer, other.m_vk_buffer);
  std::swap(m_allocation, other.m_allocation);

  return *this;
}

template <typename base_buffer_type>
buffer<base_buffer_type>::~buffer() {
  ReturnIf(m_vk_buffer == VK_NULL_HANDLE);
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& allocator = vulkan_context.mutable_resource_allocator();
  allocator.destroy_buffer(m_vk_buffer, m_allocation);
}

template <typename base_buffer_type>
void buffer<base_buffer_type>::update_data(void* data, size_t data_size) {}

template <typename base_buffer_type>
void buffer<base_buffer_type>::add_descriptor_to(
    base_renderer& renderer) /*override*/
{
  ReturnUnless(m_descriptor_build_data.m_enabled);

  auto& descriptor_manager = renderer.mutable_descriptor_set_manager();
  descriptor_manager.add_resource(m_descriptor_build_data.m_descriptor_name,
                                  *this);
}

template <typename base_buffer_type>
[[nodiscard]] VkDeviceAddress buffer<base_buffer_type>::get_address() const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  VkBufferDeviceAddressInfo info;
  std::memset(&info, 0, sizeof(VkBufferDeviceAddressInfo));

  info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  info.buffer = m_vk_buffer;

  return vkGetBufferDeviceAddress(device.get_vulkan_logical_device(), &info);
}

template class buffer<
    wunder::vulkan::shader_resource::instance::storage_buffers>;
template class buffer<
    wunder::vulkan::shader_resource::instance::uniform_buffer>;
}  // namespace wunder::vulkan