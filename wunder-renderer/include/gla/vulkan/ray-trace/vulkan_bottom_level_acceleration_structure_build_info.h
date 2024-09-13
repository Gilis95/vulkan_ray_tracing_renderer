#ifndef WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H
#define WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_build_info.h"

namespace wunder {
struct mesh_asset;
class vulkan_buffer;

class vulkan_bottom_level_acceleration_structure_build_info
    : public vulkan_acceleration_structure_build_info {
 public:
  vulkan_bottom_level_acceleration_structure_build_info(
      const mesh_asset& mesh, const vulkan_buffer& vertex_buffer,
      const vulkan_buffer& index_buffer);

 private:
  void create_geometry_data(const mesh_asset& mesh,
                            const vulkan_buffer& vertex_buffer,
                            const vulkan_buffer& index_buffer);

 public:
  [[nodiscard]] VkAccelerationStructureTypeKHR get_acceleration_structure_type()
      const override {
    return VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
  }
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_BUILD_INFO_H
