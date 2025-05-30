
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
acceleration_structure_builder<build_info_type>::acceleration_structure_builder(
    VkCommandBuffer command_buffer)
    : m_command_buffer(command_buffer) {}

template <derived<acceleration_structure_build_info> build_info_type>
acceleration_structure_builder<build_info_type>::~acceleration_structure_builder() {
  m_scratch_buffer.reset();
}

template <derived<acceleration_structure_build_info> build_info_type>
void acceleration_structure_builder<build_info_type>::create_scratch_buffer(
    std::uint32_t scratch_buffer_size) {
  m_scratch_buffer.reset(new storage_device_buffer(
      descriptor_build_data{.m_enabled = false, .m_descriptor_name = ""}, scratch_buffer_size,
      VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
}

template <derived<acceleration_structure_build_info> build_info_type>
void acceleration_structure_builder<build_info_type>::
    build_acceleration_structure(
        std::vector<const VkAccelerationStructureBuildRangeInfoKHR*>&
            as_build_offset_info,
        std::vector<VkAccelerationStructureBuildGeometryInfoKHR>&
            as_build_geometry_info) {
  auto& build_infos = get_build_infos();

  ;
  as_build_offset_info.reserve(build_infos.size());
  std::transform(build_infos.begin(), build_infos.end(),
                 std::back_insert_iterator(as_build_offset_info),
                 [](const acceleration_structure_build_info& build_info) {
                   return build_info.get_vulkan_as_build_offset_info().data();
                 });

  ;
  as_build_geometry_info.reserve(build_infos.size());
  std::transform(build_infos.begin(), build_infos.end(),
                 std::back_insert_iterator(as_build_geometry_info),
                 [](const acceleration_structure_build_info& build_info) {
                   return build_info.get_build_info();
                 });

  vkCmdBuildAccelerationStructuresKHR(
      m_command_buffer, static_cast<uint32_t>(as_build_geometry_info.size()),
      as_build_geometry_info.data(), as_build_offset_info.data());

  VkMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
  barrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
  barrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
  vkCmdPipelineBarrier(m_command_buffer,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
                       0, 1, &barrier, 0, nullptr, 0, nullptr);
}

template class acceleration_structure_builder<
    bottom_level_acceleration_structure_build_info>;

template class acceleration_structure_builder<
    top_level_acceleration_structure_build_info>;

}  // namespace wunder::vulkan
// namespace wunder::vulkan