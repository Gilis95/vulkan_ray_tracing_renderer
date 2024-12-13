#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure.h"

#include "assets/components/mesh_asset.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_types.h"

namespace wunder::vulkan {
bottom_level_acceleration_structure::bottom_level_acceleration_structure() = default;

void bottom_level_acceleration_structure::build(
    buffer& scratch_buffer, VkDeviceAddress scratch_buffer_offset,
    const bottom_level_acceleration_structure_build_info& build_info) {
  create_acceleration_structure(
      build_info.get_acceleration_structure_type(),
      build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize);
  build_acceleration_structure(scratch_buffer, scratch_buffer_offset,
                               build_info);
}
}  // namespace wunder::vulkan