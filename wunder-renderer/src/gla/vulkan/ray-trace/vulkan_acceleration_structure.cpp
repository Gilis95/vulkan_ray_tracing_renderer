#include "gla/vulkan/ray-trace/vulkan_acceleration_structure.h"

#include <cstring>

#include "core/wunder_macros.h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_shader_types.h"
#include "include/assets/mesh_asset.h"
#include "include/gla/vulkan/ray-trace/vulkan_rtx_renderer.h"

namespace wunder::vulkan {
acceleration_structure::acceleration_structure() = default;

acceleration_structure::acceleration_structure(
    acceleration_structure&& other) noexcept
    : ::wunder::vulkan::shader_resource::instance::acceleration_structure(
          std::move(other)),
      m_acceleration_structure_buffer(
          std::move(other.m_acceleration_structure_buffer)) {
  other.m_descriptor = VK_NULL_HANDLE;
}
acceleration_structure& acceleration_structure::operator=(
    acceleration_structure&& other) noexcept {
  std::swap(m_descriptor, other.m_descriptor);
  std::swap(m_acceleration_structure_buffer,
            other.m_acceleration_structure_buffer);

  return *this;
}

acceleration_structure::~acceleration_structure() {
  ReturnIf(m_descriptor == VK_NULL_HANDLE);

  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();

  vkDestroyAccelerationStructureKHR(device.get_vulkan_logical_device(),
                                    m_descriptor, VK_NULL_HANDLE);

  if (m_acceleration_structure_buffer) {
    m_acceleration_structure_buffer.reset();
  }
}

void acceleration_structure::create_acceleration_structure(
    VkAccelerationStructureTypeKHR acceleration_structure_type,
    VkDeviceSize acceleration_structure_size) {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();

  // Allocating the buffer to hold the acceleration structure
  m_acceleration_structure_buffer.reset(new storage_device_buffer(
      {.m_enabled = false, .m_descriptor_name = ""},
      acceleration_structure_size,
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT));

  VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info;
  std::memset(&acceleration_structure_create_info, 0,
              sizeof(VkAccelerationStructureCreateInfoKHR));
  acceleration_structure_create_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
  acceleration_structure_create_info.type = acceleration_structure_type;
  acceleration_structure_create_info.size = acceleration_structure_size;
  acceleration_structure_create_info.buffer =
      m_acceleration_structure_buffer->get_buffer();

  // Create the acceleration structure
  vkCreateAccelerationStructureKHR(device.get_vulkan_logical_device(),
                                   &acceleration_structure_create_info, nullptr,
                                   &m_descriptor);
}

void acceleration_structure::add_descriptor_to(descriptor_set_manager& /*target*/) {
}

VkDeviceAddress acceleration_structure::get_address() const {
  ReturnIf(vkGetAccelerationStructureDeviceAddressKHR == nullptr, 0);

  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();

  VkAccelerationStructureDeviceAddressInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
  info.accelerationStructure = m_descriptor;
  return vkGetAccelerationStructureDeviceAddressKHR(
      device.get_vulkan_logical_device(), &info);
}
}  // namespace wunder::vulkan