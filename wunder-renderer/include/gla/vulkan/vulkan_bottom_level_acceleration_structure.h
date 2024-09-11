#ifndef WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H

#include <glad/vulkan.h>
#include <vector>

namespace wunder {
struct vulkan_buffer;
struct mesh_asset;

class vulkan_bottom_level_acceleration_structure {
 private:
  struct blas_build_input {
    VkAccelerationStructureGeometryKHR m_as_geometry;
    VkAccelerationStructureBuildRangeInfoKHR m_as_build_offset_info;
    VkBuildAccelerationStructureFlagsKHR
        m_vk_build_acceleration_structure_flags{0};
  };

 public:
  void add_mesh_data(const mesh_asset& mesh,
                     const vulkan_buffer& vertex_buffer, const vulkan_buffer& index_buffer);


  void build();
 private:
  std::vector<blas_build_input> m_blas_input;
};
}
#endif  // WUNDER_VULKAN_BOTTOM_LEVEL_ACCELERATION_STRUCTURE_H
