#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_build_info.h"

#include <glad/vulkan.h>

#include <cstring>

#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "include/assets/mesh_asset.h"

namespace wunder::vulkan {
acceleration_structure_build_info::acceleration_structure_build_info() = default;

acceleration_structure_build_info::acceleration_structure_build_info(
    acceleration_structure_build_info&& other) noexcept
    : m_as_geometry(std::move(other.m_as_geometry)),
      m_as_build_offset_info(std::move(other.m_as_build_offset_info)),
      m_build_info(std::move(other.m_build_info)),
      m_build_sizes_info(std::move(other.m_build_sizes_info)) {
  m_build_info.pGeometries = &m_as_geometry;
}

acceleration_structure_build_info& acceleration_structure_build_info::operator=(
    acceleration_structure_build_info&& other) noexcept {
  std::swap(m_as_geometry, other.m_as_geometry);
  std::swap(m_as_build_offset_info, other.m_as_build_offset_info);
  std::swap(m_build_info, other.m_build_info);
  std::swap(m_build_sizes_info, other.m_build_sizes_info);

  m_build_info.pGeometries = &m_as_geometry;
}

acceleration_structure_build_info::
    ~acceleration_structure_build_info() = default;

void acceleration_structure_build_info::clear_geometry_data() {
  memset(&m_as_geometry, 0, sizeof(VkAccelerationStructureGeometryKHR));
}

void acceleration_structure_build_info::fill_range_info_data(
    std::uint32_t geometries_count) {
  m_as_build_offset_info.firstVertex = 0;
  m_as_build_offset_info.primitiveCount = geometries_count;
  m_as_build_offset_info.primitiveOffset = 0;
  m_as_build_offset_info.transformOffset = 0;
}

void acceleration_structure_build_info::create_build_info(
    VkBuildAccelerationStructureFlagsKHR build_acceleration_structure_flags) {
  std::memset(&m_build_info, 0,
              sizeof(VkAccelerationStructureBuildGeometryInfoKHR));

  m_build_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
  m_build_info.type = get_acceleration_structure_type();
  m_build_info.flags = build_acceleration_structure_flags;
  m_build_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
  m_build_info.srcAccelerationStructure = VK_NULL_HANDLE;
  m_build_info.dstAccelerationStructure = VK_NULL_HANDLE;
  m_build_info.geometryCount = 1;
  m_build_info.pGeometries = &m_as_geometry;
  m_build_info.ppGeometries = nullptr;
  m_build_info.scratchData.deviceAddress = 0;
}

void acceleration_structure_build_info::calculate_build_size() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();

  std::memset(&m_build_sizes_info, 0, sizeof m_build_sizes_info);
  m_build_sizes_info.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

  vkGetAccelerationStructureBuildSizesKHR(
      device.get_vulkan_logical_device(),
      VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &m_build_info,
      &m_as_build_offset_info.primitiveCount, &m_build_sizes_info);
}
}  // namespace wunder::vulkan