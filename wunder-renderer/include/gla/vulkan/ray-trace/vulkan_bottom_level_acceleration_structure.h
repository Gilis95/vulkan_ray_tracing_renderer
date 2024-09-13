#ifndef WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H

#include <glad/vulkan.h>

#include <vector>

#include "gla/vulkan/ray-trace/vulkan_acceleration_structure.h"

namespace wunder {
struct mesh_asset;

class vulkan_bottom_level_acceleration_structure_build_info;

class vulkan_bottom_level_acceleration_structure
    : public vulkan_acceleration_structure {
 public:
  vulkan_bottom_level_acceleration_structure();

 public:
  void build(vulkan_buffer& scratch_buffer, VkDeviceAddress scratch_offset,
             const vulkan_bottom_level_acceleration_structure_build_info& build_info);


};
}  // namespace wunder
#endif  // WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
