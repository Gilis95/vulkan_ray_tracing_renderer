

#ifndef WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
#include <glad/vulkan.h>

#include "gla/vulkan/ray-trace/vulkan_acceleration_structure.h"

namespace wunder::vulkan {

class top_level_acceleration_structure_build_info;
class rtx_renderer;

class top_level_acceleration_structure : public acceleration_structure {
 private:
  friend class top_level_acceleration_structure_builder;

 public:
  top_level_acceleration_structure();

 public:
  void add_descriptor_to(descriptor_set_manager& renderer) override;

 private:
  void create(const top_level_acceleration_structure_build_info& build_info);
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_H
