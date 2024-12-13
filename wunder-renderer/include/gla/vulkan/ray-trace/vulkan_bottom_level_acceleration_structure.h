#ifndef WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H

#include <glad/vulkan.h>

#include <vector>

#include "gla/vulkan/ray-trace/vulkan_acceleration_structure.h"

namespace wunder {
struct mesh_asset;
namespace vulkan {
class bottom_level_acceleration_structure_build_info;

class bottom_level_acceleration_structure : public acceleration_structure {
 public:
  bottom_level_acceleration_structure();

 public:
  void build(
      buffer& scratch_buffer, VkDeviceAddress scratch_offset,
      const bottom_level_acceleration_structure_build_info& build_info);
};
}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
