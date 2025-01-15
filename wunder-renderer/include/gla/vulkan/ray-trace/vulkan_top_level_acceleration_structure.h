

#ifndef WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
#include <glad/vulkan.h>

#include "gla/vulkan/ray-trace/vulkan_acceleration_structure.h"

namespace wunder::vulkan {

class top_level_acceleration_structure_build_info;
class renderer;

class top_level_acceleration_structure : public acceleration_structure {
 public:
  top_level_acceleration_structure();

 public:
  void add_descriptor_to(renderer& renderer) override;

  void build(
      storage_buffer& scratch_buffer,
      const top_level_acceleration_structure_build_info& build_info);
 private:
  void wait_until_instances_buffer_is_available() const;
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
