#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"

#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "include/gla/vulkan/ray-trace/vulkan_rtx_renderer.h"

namespace wunder::vulkan {
top_level_acceleration_structure::top_level_acceleration_structure()
    : acceleration_structure(){};

void top_level_acceleration_structure::create(
    const top_level_acceleration_structure_build_info& build_info) {
  create_acceleration_structure(
      build_info.get_acceleration_structure_type(),
      build_info.get_vulkan_as_build_sizes_info().accelerationStructureSize);
}


void top_level_acceleration_structure::add_descriptor_to(
    descriptor_set_manager& target) {
  target.add_resource("topLevelAS", *this);
}
}  // namespace wunder::vulkan