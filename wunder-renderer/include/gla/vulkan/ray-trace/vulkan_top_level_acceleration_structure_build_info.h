#ifndef WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H
#define WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H

#include <cstdint>
#include <vector>

#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_buffer.h"

namespace wunder {
class vulkan_bottom_level_acceleration_structure;
struct vulkan_mesh_scene_node;
class vulkan_top_level_acceleration_structure_build_info
    : public vulkan_acceleration_structure_build_info {
 public:
  vulkan_top_level_acceleration_structure_build_info(
      const std::vector<vulkan_mesh_scene_node>& mesh_nodes);

 public:
  [[nodiscard]] VkAccelerationStructureTypeKHR get_acceleration_structure_type()
      const override {
    return VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
  }

 private:
  static std::vector<VkAccelerationStructureInstanceKHR>
  create_acceleration_structure_instances(
      const std::vector<vulkan_mesh_scene_node>& blas);
  static bool create_acceleration_structure_instance(
      const vulkan_mesh_scene_node& blas,
      VkAccelerationStructureInstanceKHR& out_acceleration_structure_instance);

 private:
  void create_acceleration_structures_buffer(
      std::vector<VkAccelerationStructureInstanceKHR>&
          acceleration_structures_instances);
  void create_geometry_data(
      std::uint32_t acceleration_structure_instances_count);

 private:
  vulkan_buffer m_acceleration_structures_buffers;
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H
