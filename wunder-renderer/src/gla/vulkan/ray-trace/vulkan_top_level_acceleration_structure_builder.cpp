

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
        const std::vector<vulkan_mesh_scene_node>& mesh_nodes)
    : m_acceleration_structure(acceleration_structure),
      mesh_nodes(mesh_nodes) {}

void top_level_acceleration_structure_builder::build() {
  m_build_infos.emplace_back(mesh_nodes);

  AssertReturnIf(m_build_infos.empty());

  create_scratch_buffer(
      m_build_infos.front().get_vulkan_as_build_sizes_info().buildScratchSize);
  build_info_set_scratch_buffer();
  create_acceleration_structures();
  wait_until_instances_buffer_is_available();
  build_acceleration_structure();
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
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto& command_pool = device.get_command_pool();

  auto command_buffer = command_pool.get_current_compute_command_buffer();

  // Make sure the copy of the instance buffer are copied before triggering the
  // acceleration structure build
  VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;

  vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       0, 1, &barrier, 0, nullptr, 0, nullptr);

  command_pool.flush_compute_command_buffer();
}

}  // namespace wunder::vulkan