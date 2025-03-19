
#ifndef VULKAN_ACCELERATION_STRUCTURE_BUILDER_H
#define VULKAN_ACCELERATION_STRUCTURE_BUILDER_H

#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"

namespace wunder::vulkan {

template <derived<acceleration_structure_build_info> build_info_type>
class acceleration_structure_builder {
 public:
  acceleration_structure_builder(VkCommandBuffer command_buffer);
  virtual ~acceleration_structure_builder() = default;

 protected:
  void create_scratch_buffer(std::uint32_t scratch_buffer_size);
  void build_acceleration_structure(
      std::vector<const VkAccelerationStructureBuildRangeInfoKHR*>&
          as_build_offset_info,
      std::vector<VkAccelerationStructureBuildGeometryInfoKHR>&
          as_build_geometry_info);

  virtual const std::vector<build_info_type>& get_build_infos() const = 0;

 protected:
  unique_ptr<storage_buffer> m_scratch_buffer;
  VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
};
}  // namespace wunder::vulkan
#endif  // VULKAN_ACCELERATION_STRUCTURE_BUILDER_H
