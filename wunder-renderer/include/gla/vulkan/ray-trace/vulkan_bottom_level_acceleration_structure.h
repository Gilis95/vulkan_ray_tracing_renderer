#ifndef WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H

#include "gla/vulkan/ray-trace/vulkan_acceleration_structure.h"

namespace wunder {
namespace vulkan {

class bottom_level_acceleration_structure_build_info;

class bottom_level_acceleration_structure : public acceleration_structure {
 private:
  friend class bottom_level_acceleration_structure_builder;

 public:
  bottom_level_acceleration_structure();

 private:
  void create(const bottom_level_acceleration_structure_build_info& build_info);
};
}  // namespace vulkan
}  // namespace wunder
#endif  // WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
