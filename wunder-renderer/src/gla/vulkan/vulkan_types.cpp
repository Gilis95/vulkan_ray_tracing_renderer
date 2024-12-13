#include "gla/vulkan/vulkan_types.h"

namespace wunder::vulkan {
namespace shader_resource::instance {
void acceleration_structures::assign_acceleration_structure(
    VkAccelerationStructureKHR structure) {
  m_descriptor_set_acceleration_structure_khr.accelerationStructureCount = 1;
  m_descriptor_set_acceleration_structure_khr.pAccelerationStructures =
      &structure;
}
}  // namespace shader_resource::instance
}  // namespace wunder::vulkan