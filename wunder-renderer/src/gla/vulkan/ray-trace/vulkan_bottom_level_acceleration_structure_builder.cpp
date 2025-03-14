
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_builder.h"

#include <numeric>

#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/vulkan_buffer.h"

namespace wunder::vulkan {

bottom_level_acceleration_structure_builder::
    bottom_level_acceleration_structure_builder(
        vector_map<asset_handle, shared_ptr<vulkan_mesh>>& mesh_instances)
    : m_mesh_instances(mesh_instances) {}

void bottom_level_acceleration_structure_builder::build() {
  for (auto& [_, _vulkan_mesh] : m_mesh_instances) {
    m_build_infos.emplace_back(*_vulkan_mesh);
  }

  std::int32_t scratch_buffer_size = 0;
  scratch_buffer_size = std::accumulate(
      m_build_infos.begin(), m_build_infos.end(), scratch_buffer_size,
      [](std::uint32_t current_accumulation,
         const acceleration_structure_build_info& right) {
        return current_accumulation +
               right.get_vulkan_as_build_sizes_info().accelerationStructureSize;
      });

  create_scratch_buffer(scratch_buffer_size);
  build_info_set_scratch_buffer();
  create_acceleration_structures();
  build_acceleration_structure();
}

void bottom_level_acceleration_structure_builder::
    build_info_set_scratch_buffer() {
  std::uint32_t scratch_buffer_offset = 0;
  for (auto& [id, mesh_instance_ptr] : m_mesh_instances) {
    AssertContinueUnless(mesh_instance_ptr);
    vulkan_mesh& mesh_instance = *mesh_instance_ptr;

    AssertContinueUnless(m_build_infos.size() > mesh_instance.m_idx);
    auto& build_info = m_build_infos[mesh_instance.m_idx];
    build_info.mutable_build_info().scratchData.deviceAddress =
        m_scratch_buffer->get_address() + scratch_buffer_offset;

    scratch_buffer_offset +=
        build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize;
  }
}

void bottom_level_acceleration_structure_builder::
    create_acceleration_structures() {
  for (auto& [_, mesh_instance_ptr] : m_mesh_instances) {
    AssertContinueUnless(mesh_instance_ptr);
    vulkan_mesh& mesh_instance = *mesh_instance_ptr;

    AssertContinueUnless(m_build_infos.size() > mesh_instance.m_idx);
    auto& build_info = m_build_infos[mesh_instance.m_idx];

    mesh_instance.m_blas.create(build_info);

    build_info.mutable_build_info().dstAccelerationStructure =
        mesh_instance.m_blas.m_descriptor;
  }
}
}  // namespace wunder::vulkan