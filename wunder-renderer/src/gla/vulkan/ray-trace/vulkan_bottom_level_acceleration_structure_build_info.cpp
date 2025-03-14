
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"

#include <glad/vulkan.h>

#include <cstring>

#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "include/assets/mesh_asset.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
bottom_level_acceleration_structure_build_info::
    bottom_level_acceleration_structure_build_info(
        const vulkan_mesh& vulkan_mesh) {
  AssertReturnUnless(vulkan_mesh.m_vertex_buffer);
  AssertReturnUnless(vulkan_mesh.m_index_buffer);

  clear_geometry_data();
  create_geometry_data(vulkan_mesh.m_vertices_count,
                       *vulkan_mesh.m_vertex_buffer,
                       *vulkan_mesh.m_index_buffer);

  std::uint32_t build_flags =
      VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;

  /**
   * Offset data, this will indicate to the GPU where it could find vertex
   * positions. In our scenario positions are the beginning of VertexAttributes
   * structure, so we don't have to set any offset
   */
  fill_range_info_data(vulkan_mesh.m_indices_count / 3);
  create_build_info(build_flags);
  calculate_build_size();
}

/**
 * We're covering only triangle meshes for the moment.
 * TODO:: implement procedural geometries, such as spheres
 */
void bottom_level_acceleration_structure_build_info::create_geometry_data(
    std::int32_t vertices_count, const storage_buffer& vertex_buffer,
    const storage_buffer& index_buffer) {  // Building part
  VkDeviceAddress vertexAddress = vertex_buffer.get_address();
  VkDeviceAddress indexAddress = index_buffer.get_address();
  // triangles.transformData = ({});

  // Setting up the build info of the acceleration
  std::memset(&m_as_geometry, 0, sizeof(VkAccelerationStructureGeometryKHR));
  m_as_geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  m_as_geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
  m_as_geometry.flags =
      VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR;  // For AnyHit

  std::memset(&m_as_geometry.geometry.triangles, 0,
              sizeof(VkAccelerationStructureGeometryTrianglesDataKHR));
  m_as_geometry.geometry.triangles.sType =
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
  m_as_geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
  m_as_geometry.geometry.triangles.vertexData.deviceAddress = vertexAddress;
  m_as_geometry.geometry.triangles.vertexStride = sizeof(VertexAttributes);
  m_as_geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
  m_as_geometry.geometry.triangles.indexData.deviceAddress = indexAddress;
  m_as_geometry.geometry.triangles.maxVertex = vertices_count;
}

}  // namespace wunder::vulkan