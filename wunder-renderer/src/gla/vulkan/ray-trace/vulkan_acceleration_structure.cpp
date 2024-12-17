#include "gla/vulkan/ray-trace/vulkan_acceleration_structure.h"

#include <cstring>

#include "assets/components/mesh_asset.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_renderer.h"
#include "gla/vulkan/vulkan_types.h"

namespace wunder::vulkan {
acceleration_structure::acceleration_structure() = default;

acceleration_structure::acceleration_structure(
    acceleration_structure&& other) noexcept
    : ::wunder::vulkan::shader_resource::instance::acceleration_structure(std::move(other)),
      m_acceleration_structure_buffer(
          std::move(other.m_acceleration_structure_buffer)) {
  other.m_descriptor = VK_NULL_HANDLE;
}
acceleration_structure& acceleration_structure::operator=(
    acceleration_structure&& other) noexcept {
  std::swap(m_descriptor, other.m_descriptor);
  std::swap(m_acceleration_structure_buffer,
            other.m_acceleration_structure_buffer);
}

acceleration_structure::~acceleration_structure() {
  ReturnIf(m_descriptor == VK_NULL_HANDLE);

  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();

  vkDestroyAccelerationStructureKHR(device.get_vulkan_logical_device(),
                                    m_descriptor, VK_NULL_HANDLE);
}

void acceleration_structure::create_acceleration_structure(
    VkAccelerationStructureTypeKHR acceleration_structure_type,
    VkDeviceSize acceleration_structure_size) {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();

  // Allocating the buffer to hold the acceleration structure
  m_acceleration_structure_buffer =
      device_buffer(
      acceleration_structure_size,
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT);

  VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info{
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR};
  acceleration_structure_create_info.type = acceleration_structure_type;
  acceleration_structure_create_info.size = acceleration_structure_size;
  acceleration_structure_create_info.buffer =
      m_acceleration_structure_buffer.get_buffer();

  // Create the acceleration structure
  vkCreateAccelerationStructureKHR(device.get_vulkan_logical_device(),
                                   &acceleration_structure_create_info, nullptr,
                                   &m_descriptor);
}

void acceleration_structure::build_acceleration_structure(
    const buffer& scratch_buffer, VkDeviceAddress scratch_buffer_offset,
    const acceleration_structure_build_info& build_info) const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  auto& command_pool = device.get_command_pool();

  std::vector<VkAccelerationStructureBuildRangeInfoKHR> as_build_offset_info = {
      build_info.get_vulkan_as_build_offset_info()};
  VkAccelerationStructureBuildRangeInfoKHR* tmp = as_build_offset_info.data();

  auto vulkan_build_info = build_info.get_build_info();
  vulkan_build_info.dstAccelerationStructure = this->m_descriptor;
  vulkan_build_info.scratchData.deviceAddress =
      scratch_buffer.get_address() + scratch_buffer_offset;

  auto command_buffer = command_pool.get_current_compute_command_buffer();
  vkCmdBuildAccelerationStructuresKHR(command_buffer, 1, &vulkan_build_info,
                                      &tmp);

  VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
  barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
  barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
  vkCmdPipelineBarrier(command_buffer,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       0, 1, &barrier, 0, nullptr, 0, nullptr);

  command_pool.flush_compute_command_buffer();
}

void acceleration_structure::bind(renderer& renderer) {}

VkDeviceAddress acceleration_structure::get_address() {
  ReturnIf(vkGetAccelerationStructureDeviceAddressKHR == nullptr, 0);

  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();

  VkAccelerationStructureDeviceAddressInfoKHR info{
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR};
  info.accelerationStructure = m_descriptor;
  return vkGetAccelerationStructureDeviceAddressKHR(
      device.get_vulkan_logical_device(), &info);
}
}  // namespace wunder::vulkan