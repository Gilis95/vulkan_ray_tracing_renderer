#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"

#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"

namespace wunder {
vulkan_top_level_acceleration_structure::
    vulkan_top_level_acceleration_structure() : vulkan_acceleration_structure(){ };

void vulkan_top_level_acceleration_structure::build(
    vulkan_buffer& scratch_buffer,
    const vulkan_top_level_acceleration_structure_build_info& build_info) {
  create_acceleration_structure(
      build_info.get_acceleration_structure_type(),
      build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize);
  build_acceleration_structure(scratch_buffer, 0, build_info);
}
}  // namespace wunder