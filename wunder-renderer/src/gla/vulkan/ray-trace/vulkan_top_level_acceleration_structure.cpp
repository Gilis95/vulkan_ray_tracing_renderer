#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"

#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_renderer.h"

namespace wunder::vulkan {
top_level_acceleration_structure::top_level_acceleration_structure()
    : acceleration_structure(){};

void top_level_acceleration_structure::build(
    buffer& scratch_buffer,
    const top_level_acceleration_structure_build_info& build_info) {
  create_acceleration_structure(
      build_info.get_acceleration_structure_type(),
      build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize);
  build_acceleration_structure(scratch_buffer, 0, build_info);
}

void top_level_acceleration_structure::bind(renderer& renderer) {
  auto& descriptor_manager = renderer.get_descriptor_set_manager();
  descriptor_manager.update_resource("topLevelAS", *this);
}
}  // namespace wunder::vulkan