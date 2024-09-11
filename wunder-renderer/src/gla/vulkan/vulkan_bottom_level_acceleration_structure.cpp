#include "gla/vulkan/vulkan_bottom_level_acceleration_structure.h"

#include <cstring>

#include "assets/components/mesh_asset.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_types.h"
#include "resources/shaders/host_device.h"

namespace wunder {

void vulkan_bottom_level_acceleration_structure::add_mesh_data(
    const mesh_asset& mesh, const vulkan_buffer& vertex_buffer,
    const vulkan_buffer& index_buffer) {
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();

  // Building part
  VkBufferDeviceAddressInfo info{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
  info.buffer = vertex_buffer.m_vk_buffer;
  VkDeviceAddress vertexAddress =
      vkGetBufferDeviceAddress(device.get_vulkan_logical_device(), &info);
  info.buffer = index_buffer.m_vk_buffer;
  VkDeviceAddress indexAddress =
      vkGetBufferDeviceAddress(device.get_vulkan_logical_device(), &info);

  VkAccelerationStructureGeometryTrianglesDataKHR triangles{
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR};
  triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
  triangles.vertexData.deviceAddress = vertexAddress;
  triangles.vertexStride = sizeof(VertexAttributes);
  triangles.indexType = VK_INDEX_TYPE_UINT32;
  triangles.indexData.deviceAddress = indexAddress;
  triangles.maxVertex = mesh.m_verticies.size();
  // triangles.transformData = ({});

  blas_build_input input;

  // Setting up the build info of the acceleration
  memset(&input.m_as_geometry, 0, sizeof input.m_as_geometry);
  input.m_as_geometry.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  input.m_as_geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  input.m_as_geometry.flags =
      VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;  // For AnyHit
  input.m_as_geometry.geometry.triangles = triangles;

  input.m_as_build_offset_info.firstVertex = 0;
  input.m_as_build_offset_info.primitiveCount = mesh.m_indecies.size() / 3;
  input.m_as_build_offset_info.primitiveOffset = 0;
  input.m_as_build_offset_info.transformOffset = 0;

  m_blas_input.push_back(std::move(input));
}

void vulkan_bottom_level_acceleration_structure::build() {
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();

  std::uint32_t build_flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR |
      VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;

  for (const auto& input : m_blas_input) {
    VkAccelerationStructureBuildGeometryInfoKHR buildInfo{};
    std::memset(&buildInfo, 0, sizeof buildInfo);

    buildInfo.sType =
        VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    buildInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    buildInfo.flags =
        build_flags | input.m_vk_build_acceleration_structure_flags;
    buildInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    buildInfo.srcAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.dstAccelerationStructure = VK_NULL_HANDLE;
    buildInfo.geometryCount = 1;
    buildInfo.pGeometries = &input.m_as_geometry;
    buildInfo.ppGeometries = nullptr;
    buildInfo.scratchData.deviceAddress = 0;

    VkAccelerationStructureBuildSizesInfoKHR
        acceleration_structure_build_sizes_info;
    std::memset(&acceleration_structure_build_sizes_info, 0,
                sizeof acceleration_structure_build_sizes_info);
    acceleration_structure_build_sizes_info.sType =
        VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

    vkGetAccelerationStructureBuildSizesKHR(
        device.get_vulkan_logical_device(),
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &buildInfo,
        &input.m_as_build_offset_info.primitiveCount,
        &acceleration_structure_build_sizes_info);
  }
}

}  // namespace wunder