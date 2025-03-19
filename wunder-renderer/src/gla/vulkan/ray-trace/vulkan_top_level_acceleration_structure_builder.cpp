#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_builder.h"

#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

namespace wunder::vulkan {

top_level_acceleration_structure_builder::
    top_level_acceleration_structure_builder(
        top_level_acceleration_structure& acceleration_structure,
        std::vector<top_level_acceleration_structure_build_info>& build_infos,
        const std::vector<vulkan_mesh_scene_node>& mesh_nodes)
    : acceleration_structure_builder(layer_abstraction_factory::instance()
                                         .get_vulkan_context()
                                         .mutable_device()
                                         .get_command_pool()
                                         .get_current_compute_command_buffer()),
      m_acceleration_structure(acceleration_structure),
      m_build_infos(build_infos),
      mesh_nodes(mesh_nodes) {}

void top_level_acceleration_structure_builder::build() {
  m_build_infos.push_back(std::move(top_level_acceleration_structure_build_info(
      m_command_buffer, mesh_nodes)));

  AssertReturnIf(m_build_infos.empty());

  create_scratch_buffer(
      m_build_infos.front().get_vulkan_as_build_sizes_info().buildScratchSize);
  build_info_set_scratch_buffer();

  wait_until_instances_buffer_is_available();

  create_acceleration_structures();

  std::vector<const VkAccelerationStructureBuildRangeInfoKHR*>
      as_build_offset_info;
  std::vector<VkAccelerationStructureBuildGeometryInfoKHR>
      as_build_geometry_info;
  build_acceleration_structure(as_build_offset_info, as_build_geometry_info);

  flush_commands();
  free_staging_data();
}

void top_level_acceleration_structure_builder::build_info_set_scratch_buffer() {
  AssertReturnIf(m_build_infos.empty());
  std::uint32_t scratch_buffer_offset = 0;

  auto& build_info = m_build_infos.front();
  build_info.mutable_build_info().scratchData.deviceAddress =
      m_scratch_buffer->get_address() + scratch_buffer_offset;
}

void top_level_acceleration_structure_builder::
    create_acceleration_structures() {
  AssertReturnIf(m_build_infos.empty());

  auto& build_info = m_build_infos.front();
  m_acceleration_structure.create(build_info);

  build_info.mutable_build_info().dstAccelerationStructure =
      m_acceleration_structure.m_descriptor;
}

void top_level_acceleration_structure_builder::
    wait_until_instances_buffer_is_available() const {
  VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
  vkCmdPipelineBarrier(m_command_buffer,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       0, 1, &barrier, 0, nullptr, 0, nullptr);
}

void top_level_acceleration_structure_builder::flush_commands() {
  auto& context = layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = context.mutable_device();

  device.get_command_pool().flush_compute_command_buffer();
}

void top_level_acceleration_structure_builder::free_staging_data() {
  for (auto& build_info : m_build_infos) {
    build_info.free_staging_data();
  }
}

}  // namespace wunder::vulkan