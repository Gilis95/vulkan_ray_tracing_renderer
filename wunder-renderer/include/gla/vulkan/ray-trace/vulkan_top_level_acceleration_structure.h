

#ifndef WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
#include <glad/vulkan.h>

#include "gla/vulkan/ray-trace/vulkan_acceleration_structure.h"

namespace wunder {
class vulkan_top_level_acceleration_structure_build_info;
class vulkan_top_level_acceleration_structure
    : public vulkan_acceleration_structure {
 public:
  vulkan_top_level_acceleration_structure();

 public:
  void build(vulkan_buffer& scratch_buffer,
             const vulkan_top_level_acceleration_structure_build_info& build_info);
};
}  // namespace wunder

#endif  // WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
