
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_builder.h"

#include <numeric>

#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/scene/vulkan_mesh_scene_node.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

namespace wunder::vulkan {

bottom_level_acceleration_structure_builder::
    bottom_level_acceleration_structure_builder(
        std::vector<vulkan_mesh_scene_node>& mesh_nodes)
    : acceleration_structure_builder(layer_abstraction_factory::instance()
                                         .get_vulkan_context()
                                         .mutable_command_pool()
                                         .get_current_compute_command_buffer()),
      m_mesh_nodes(mesh_nodes),
      m_min_alignment(128) {}

void bottom_level_acceleration_structure_builder::build() {
  for (auto& [vulkan_mesh, _] : m_mesh_nodes) {
    m_build_infos.emplace_back(*vulkan_mesh);
  }

  std::int32_t scratch_buffer_size = 0;

  scratch_buffer_size = std::accumulate(
      m_build_infos.begin(), m_build_infos.end(), scratch_buffer_size,
      [min_alignment = m_min_alignment](
          std::uint32_t current_accumulation,
          const acceleration_structure_build_info& right) {
        return current_accumulation +
               align_up(right.get_vulkan_as_build_sizes_info().buildScratchSize,
                        min_alignment);
      });

  create_scratch_buffer(scratch_buffer_size);
  build_info_set_scratch_buffer();
  create_acceleration_structures();

  std::vector<const VkAccelerationStructureBuildRangeInfoKHR*>
      as_build_offset_info;
  std::vector<VkAccelerationStructureBuildGeometryInfoKHR>
      as_build_geometry_info;
  build_acceleration_structure(as_build_offset_info, as_build_geometry_info);

  flush_commands();
}

void bottom_level_acceleration_structure_builder::
    build_info_set_scratch_buffer() {
  std::uint32_t scratch_buffer_offset = 0;
  for (auto& build_info : m_build_infos) {
    auto& vk_acceleration_structure_build_geometry_info_khr =
        build_info.mutable_build_info();
    vk_acceleration_structure_build_geometry_info_khr.scratchData
        .deviceAddress =
        m_scratch_buffer->get_address() + scratch_buffer_offset;

    scratch_buffer_offset += static_cast<uint32_t>(align_up(
        build_info.get_vulkan_as_build_sizes_info().buildScratchSize, 128));
  }
}

void bottom_level_acceleration_structure_builder::
    create_acceleration_structures() {
  for (auto& [mesh_instance_ptr, _] : m_mesh_nodes) {
    AssertContinueUnless(mesh_instance_ptr);
    vulkan_mesh& mesh_instance = *mesh_instance_ptr;

    AssertContinueUnless(m_build_infos.size() > mesh_instance.m_idx);
    auto& build_info = m_build_infos[mesh_instance.m_idx];

    mesh_instance.m_blas.create(build_info);

    build_info.mutable_build_info().dstAccelerationStructure =
        mesh_instance.m_blas.m_descriptor;
  }
}

void bottom_level_acceleration_structure_builder::flush_commands() {
  auto& context = layer_abstraction_factory::instance().get_vulkan_context();
  context.mutable_command_pool().flush_compute_command_buffer();
}
}  // namespace wunder::vulkan