#ifndef WUNDER_VULKAN_ACCELERATION_STRUCTURE_BUILD_INFO_H
#define WUNDER_VULKAN_ACCELERATION_STRUCTURE_BUILD_INFO_H
#include <glad/vulkan.h>

#include <cstdint>

#include "core/non_copyable.h"
namespace wunder::vulkan {
class acceleration_structure_build_info : public non_copyable {
 public:
  acceleration_structure_build_info();

  acceleration_structure_build_info(
      acceleration_structure_build_info&& other) noexcept;
  acceleration_structure_build_info& operator=(
      acceleration_structure_build_info&& other) noexcept;

 public:
  virtual ~acceleration_structure_build_info();

 public:
  [[nodiscard]]  VkAccelerationStructureBuildGeometryInfoKHR&
    mutable_build_info()  {
    return m_build_info;
  }

  [[nodiscard]] const VkAccelerationStructureBuildGeometryInfoKHR&
  get_build_info() const {
    return m_build_info;
  }

  [[nodiscard]] const VkAccelerationStructureBuildRangeInfoKHR&
  get_vulkan_as_build_offset_info() const {
    return m_as_build_offset_info;
  }

  [[nodiscard]] const VkAccelerationStructureBuildSizesInfoKHR&
  get_vulkan_as_build_sizes_info() const {
    return m_build_sizes_info;
  }

 public:
  [[nodiscard]] virtual VkAccelerationStructureTypeKHR
  get_acceleration_structure_type() const = 0;

 protected:
  void clear_geometry_data();
  void fill_range_info_data(std::uint32_t geometries_count);
  void create_build_info(
      VkBuildAccelerationStructureFlagsKHR build_acceleration_structure_flags);

 protected:
  void calculate_build_size();

 protected:
  VkAccelerationStructureGeometryKHR m_as_geometry{};
  VkAccelerationStructureBuildRangeInfoKHR m_as_build_offset_info{};
  VkAccelerationStructureBuildGeometryInfoKHR m_build_info{};
  VkAccelerationStructureBuildSizesInfoKHR m_build_sizes_info{};
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_ACCELERATION_STRUCTURE_BUILD_INFO_H
