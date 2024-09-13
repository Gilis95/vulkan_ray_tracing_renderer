//
// Created by christian on 9/12/24.
//

#ifndef WUNDER_VULKAN_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_ACCELERATION_STRUCTURE_H

#include <glad/vulkan.h>
#include "gla/vulkan/vulkan_buffer.h"

namespace wunder {
class vulkan_acceleration_structure_build_info;

class vulkan_acceleration_structure : public non_copyable{
 public:
  vulkan_acceleration_structure();
  virtual ~vulkan_acceleration_structure();

  vulkan_acceleration_structure(vulkan_acceleration_structure&&) noexcept;
  vulkan_acceleration_structure& operator=(vulkan_acceleration_structure&&) noexcept;

 public:
  VkDeviceAddress get_address();
 protected:
  void create_acceleration_structure(
      VkAccelerationStructureTypeKHR acceleration_structure_type,
      VkDeviceSize acceleration_structure_size);

  void build_acceleration_structure(
      const vulkan_buffer& scratch_buffer,
      VkDeviceAddress scratch_buffer_offset,
      const vulkan_acceleration_structure_build_info& build_info) const;
 protected:

  VkAccelerationStructureKHR m_acceleration_structure = VK_NULL_HANDLE;
  vulkan_buffer m_acceleration_structure_buffer;
};
}
#endif  // WUNDER_VULKAN_ACCELERATION_STRUCTURE_H
