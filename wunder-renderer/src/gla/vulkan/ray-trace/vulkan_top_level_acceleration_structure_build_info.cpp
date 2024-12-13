#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"

#include <cstring>

#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure.h"
#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/scene/vulkan_mesh_scene_node.h"
#include "gla/vulkan/vulkan_device_buffer.h"
namespace {
inline VkTransformMatrixKHR to_transform_matrix_khr(glm::mat4 matrix) {
  // VkTransformMatrixKHR uses a row-major memory layout, while glm::mat4
  // uses a column-major memory layout. We transpose the matrix so we can
  // memcpy the matrix's data directly.
  glm::mat4 temp = glm::transpose(matrix);
  VkTransformMatrixKHR out_matrix;
  std::memcpy(&out_matrix, &temp, sizeof(VkTransformMatrixKHR));
  return out_matrix;
}
}  // namespace

namespace wunder::vulkan {

top_level_acceleration_structure_build_info::
    top_level_acceleration_structure_build_info(
        const std::vector<vulkan_mesh_scene_node>& mesh_nodes) {
  auto acceleration_structures_instances =
      create_acceleration_structure_instances(mesh_nodes);
  create_acceleration_structures_buffer(acceleration_structures_instances);

  clear_geometry_data();
  create_geometry_data(acceleration_structures_instances.size());

  fill_range_info_data(acceleration_structures_instances.size());

  auto build_flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
  create_build_info(build_flags);
  calculate_build_size();
}

std::vector<VkAccelerationStructureInstanceKHR>
top_level_acceleration_structure_build_info::
    create_acceleration_structure_instances(
        const std::vector<vulkan_mesh_scene_node>& mesh_nodes) {
  std::vector<VkAccelerationStructureInstanceKHR> result;
  for (auto& mesh_node : mesh_nodes) {
    VkAccelerationStructureInstanceKHR acceleration_structure_instance;
    AssertContinueUnless(create_acceleration_structure_instance(
        mesh_node, acceleration_structure_instance));
    result.emplace_back(acceleration_structure_instance);
  }

  return result;
}
bool top_level_acceleration_structure_build_info::
    create_acceleration_structure_instance(
        const vulkan_mesh_scene_node& mesh_node,
        VkAccelerationStructureInstanceKHR&
            out_acceleration_structure_instance) {
  AssertReturnUnless(mesh_node.m_mesh, false);

  VkGeometryInstanceFlagsKHR flags{};

  // Always opaque, no need to use anyhit (faster)
  //  if(mat.alphaMode == 0 || (mat.baseColorFactor.w == 1.0f &&
  //  mat.baseColorTexture == -1))
  flags |= VK_GEOMETRY_INSTANCE_FORCE_OPAQUE_BIT_KHR;
  // Need to skip the cull flag in traceray_rtx for double sided materials
  //  if(mat.doubleSided == 1)
  flags |= VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;

  out_acceleration_structure_instance.transform =
      to_transform_matrix_khr(mesh_node.m_model_matrix);
  out_acceleration_structure_instance.instanceCustomIndex =
      mesh_node.m_mesh->m_material_idx;  // gl_InstanceCustomIndexEXT: to find
                                         // which primitive
  out_acceleration_structure_instance.accelerationStructureReference =
      mesh_node.m_mesh->m_blas.get_address();
  out_acceleration_structure_instance.flags = flags;
  out_acceleration_structure_instance.instanceShaderBindingTableRecordOffset =
      0;  // We will use the same hit group for all objects
  out_acceleration_structure_instance.mask = 0xFF;
  return true;
}

void top_level_acceleration_structure_build_info::
    create_acceleration_structures_buffer(
        std::vector<VkAccelerationStructureInstanceKHR>&
            acceleration_structures_instances) {
  VkAccelerationStructureInstanceKHR* data =
      acceleration_structures_instances.data();
  unsigned long size = acceleration_structures_instances.size() *
                       sizeof(VkAccelerationStructureInstanceKHR);
  VkBufferUsageFlagBits flags = VkBufferUsageFlagBits(
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
      VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
  m_acceleration_structures_buffers = device_buffer(data, size, flags);
}

void top_level_acceleration_structure_build_info::create_geometry_data(
    std::uint32_t acceleration_structure_instances_count) {
  VkAccelerationStructureGeometryInstancesDataKHR geometryInstances{
      VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR};
  geometryInstances.data.deviceAddress =
      m_acceleration_structures_buffers.get_address();

  // Set up the geometry to use instance data.
  m_as_geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
  m_as_geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
  m_as_geometry.geometry.instances = geometryInstances;
}
}  // namespace wunder::vulkan