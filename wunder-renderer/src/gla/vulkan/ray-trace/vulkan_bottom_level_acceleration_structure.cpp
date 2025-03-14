#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure.h"

#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_device.h"

namespace wunder::vulkan {
bottom_level_acceleration_structure::bottom_level_acceleration_structure() =
    default;

void bottom_level_acceleration_structure::create(
    const bottom_level_acceleration_structure_build_info& build_info) {
    create_acceleration_structure(
        build_info.get_acceleration_structure_type(),
        build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize);

}
}  // namespace wunder::vulkan