#ifndef WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H
#define WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H

#include <cstdint>
#include <vector>

#include "core/wunder_memory.h"
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"

namespace wunder::vulkan {

class bottom_level_acceleration_structure;
struct vulkan_mesh_scene_node;

class top_level_acceleration_structure_build_info
    : public acceleration_structure_build_info {
 public:
  top_level_acceleration_structure_build_info(
      VkCommandBuffer command_buffer, const std::vector<vulkan_mesh_scene_node>& mesh_nodes);
  ~top_level_acceleration_structure_build_info() override;


  top_level_acceleration_structure_build_info(
      top_level_acceleration_structure_build_info&& other) noexcept;
  top_level_acceleration_structure_build_info& operator=(
      top_level_acceleration_structure_build_info&& other) noexcept;

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
  VkCommandBuffer m_command_buffer;
  unique_ptr<storage_buffer> m_acceleration_structures_buffers;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H
