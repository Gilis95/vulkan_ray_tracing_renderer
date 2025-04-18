#ifndef VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_BUILDER_H
#define VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_BUILDER_H
#include <vector>

#include "assets/asset_types.h"
#include "core/vector_map.h"
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_builder.h"
#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure_build_info.h"

namespace wunder::vulkan {
struct vulkan_mesh_scene_node;
struct vulkan_mesh;
}  // namespace wunder::vulkan

namespace wunder::vulkan {
class bottom_level_acceleration_structure_builder final
    : protected acceleration_structure_builder<bottom_level_acceleration_structure_build_info> {
public:
  bottom_level_acceleration_structure_builder(
      std::vector<vulkan_mesh_scene_node>& mesh_nodes);

 public:
  void build();
protected:

  const std::vector<bottom_level_acceleration_structure_build_info>&
        get_build_infos() const override {
    return m_build_infos;
  }
 private:
  void build_info_set_scratch_buffer();
  void create_acceleration_structures();
  void flush_commands();

 private:
  std::vector<bottom_level_acceleration_structure_build_info>
      m_build_infos;
  std::vector<vulkan_mesh_scene_node>& m_mesh_nodes;
  uint32_t m_min_alignment ; /*VkPhysicalDeviceAccelerationStructurePropertiesKHR.minAccelerationStructureScratchOffsetAlignment*/

};
}  // namespace wunder::vulkan
#endif  // VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_BUILDER_H
