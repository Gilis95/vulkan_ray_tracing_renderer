#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"

#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_renderer.h"

namespace wunder::vulkan {
top_level_acceleration_structure::top_level_acceleration_structure()
    : acceleration_structure(){};

void top_level_acceleration_structure::build(
    storage_buffer& scratch_buffer,
    const top_level_acceleration_structure_build_info& build_info) {
  create_acceleration_structure(
      build_info.get_acceleration_structure_type(),
      build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize);
  wait_until_instances_buffer_is_available();
  build_acceleration_structure(scratch_buffer, 0, build_info);
}

void top_level_acceleration_structure::
    wait_until_instances_buffer_is_available() const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
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

void top_level_acceleration_structure::add_descriptor_to(renderer& renderer) {
  auto& descriptor_manager = renderer.get_descriptor_set_manager();
  descriptor_manager.add_resource("topLevelAS", *this);
}
}  // namespace wunder::vulkan