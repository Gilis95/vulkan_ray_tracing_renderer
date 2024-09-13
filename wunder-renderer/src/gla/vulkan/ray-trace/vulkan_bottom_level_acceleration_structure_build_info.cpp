
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"

#include <glad/vulkan.h>

#include <cstring>

#include "assets/components/mesh_asset.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "resources/shaders/host_device.h"

namespace wunder {
vulkan_bottom_level_acceleration_structure_build_info::
    vulkan_bottom_level_acceleration_structure_build_info(
        const mesh_asset& mesh, const vulkan_buffer& vertex_buffer,
        const vulkan_buffer& index_buffer) {
  clear_geometry_data();
  create_geometry_data(mesh, vertex_buffer, index_buffer);

  std::uint32_t build_flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR |
      VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR;

  /**
   * Offset data, this will indicate to the GPU where it could find vertex
   * positions. In our scenario positions are the beginning of VertexAttributes
   * structure, so we don't have to set any offset
   */
  fill_range_info_data(mesh.m_indecies.size() / 3);
  create_build_info(build_flags);
  calculate_build_size();
}

/**
 * We're covering only triangle meshes for the moment.
 * TODO:: implement procedural geometries, such as spheres
 */
void vulkan_bottom_level_acceleration_structure_build_info::
    create_geometry_data(const mesh_asset& mesh,
                         const vulkan_buffer& vertex_buffer,
                         const vulkan_buffer& index_buffer) {  // Building part
  vulkan_context& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();

  VkBufferDeviceAddressInfo info{VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO};
  info.buffer = vertex_buffer.get_buffer();
  VkDeviceAddress vertexAddress =
      vkGetBufferDeviceAddress(device.get_vulkan_logical_device(), &info);
  info.buffer = index_buffer.get_buffer();
  VkDeviceAddress indexAddress =
      vkGetBufferDeviceAddress(device.get_vulkan_logical_device(), &info);

  VkAccelerationStructureGeometryTrianglesDataKHR triangles;
  std::memset(&triangles, 0,
              sizeof(VkAccelerationStructureGeometryTrianglesDataKHR));
  triangles.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
  triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
  triangles.vertexData.deviceAddress = vertexAddress;
  triangles.vertexStride = sizeof(VertexAttributes);
  triangles.indexType = VK_INDEX_TYPE_UINT32;
  triangles.indexData.deviceAddress = indexAddress;
  triangles.maxVertex = mesh.m_verticies.size();
  // triangles.transformData = ({});

  // Setting up the build info of the acceleration
  m_as_geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  m_as_geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  m_as_geometry.flags =
      VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;  // For AnyHit
  m_as_geometry.geometry.triangles = triangles;
}

}  // namespace wunder