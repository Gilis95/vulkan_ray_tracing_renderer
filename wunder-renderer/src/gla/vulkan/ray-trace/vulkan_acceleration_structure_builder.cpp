
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_builder.h"

#include <numeric>

#include "assets/asset_types.h"
#include "core/vector_map.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/scene/vulkan_mesh.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

namespace wunder::vulkan {
template <derived<acceleration_structure_build_info> build_info_type>
vulkan_acceleration_structure_builder<
    build_info_type>::vulkan_acceleration_structure_builder() = default;

template <derived<acceleration_structure_build_info> build_info_type>
void vulkan_acceleration_structure_builder<
    build_info_type>::create_scratch_buffer(std::uint32_t scratch_buffer_size) {
  auto& build_infos = get_build_infos();

  m_scratch_buffer.reset(new storage_device_buffer(
      descriptor_build_data{.m_enabled = false}, scratch_buffer_size,
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
}

template <derived<acceleration_structure_build_info> build_info_type>
void vulkan_acceleration_structure_builder<
    build_info_type>::build_acceleration_structure() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto& command_pool = device.get_command_pool();
  auto& build_infos = get_build_infos();

  std::vector<VkAccelerationStructureBuildRangeInfoKHR> as_build_offset_info;
  std::transform(build_infos.begin(), build_infos.end(),
                 std::back_insert_iterator(as_build_offset_info),
                 [](const acceleration_structure_build_info& build_info) {
                   return build_info.get_vulkan_as_build_offset_info();
                 });

  VkAccelerationStructureBuildRangeInfoKHR* tmp = as_build_offset_info.data();

  std::vector<VkAccelerationStructureBuildGeometryInfoKHR>
      as_build_geometry_info;
  std::transform(build_infos.begin(), build_infos.end(),
                 std::back_insert_iterator(as_build_geometry_info),
                 [](const acceleration_structure_build_info& build_info) {
                   return build_info.get_build_info();
                 });

  auto command_buffer = command_pool.get_current_compute_command_buffer();
  vkCmdBuildAccelerationStructuresKHR(command_buffer, 1,
                                      as_build_geometry_info.data(), &tmp);

  VkMemoryBarrier barrier{VK_STRUCTURE_TYPE_MEMORY_BARRIER};
  barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
  barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
  vkCmdPipelineBarrier(command_buffer,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       0, 1, &barrier, 0, nullptr, 0, nullptr);

  command_pool.flush_compute_command_buffer();
}

template class vulkan_acceleration_structure_builder<
    bottom_level_acceleration_structure_build_info>;

template class vulkan_acceleration_structure_builder<
    top_level_acceleration_structure_build_info>;

}  // namespace wunder::vulkan
// namespace wunder::vulkan