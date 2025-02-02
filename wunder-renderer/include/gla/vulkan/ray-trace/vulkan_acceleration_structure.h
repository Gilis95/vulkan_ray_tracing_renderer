//
// Created by christian on 9/12/24.
//

#ifndef WUNDER_VULKAN_ACCELERATION_STRUCTURE_H
#define WUNDER_VULKAN_ACCELERATION_STRUCTURE_H

#include <glad/vulkan.h>

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"
#include "gla/vulkan/vulkan_shader_types.h"

namespace wunder::vulkan {
class acceleration_structure_build_info;

class acceleration_structure
    : public non_copyable,
      public vulkan::shader_resource::instance::acceleration_structure {
 public:
  acceleration_structure();
  virtual ~acceleration_structure();

  acceleration_structure(acceleration_structure&&) noexcept;
  acceleration_structure& operator=(acceleration_structure&&) noexcept;

 public:
  void add_descriptor_to(base_renderer& renderer) override;
  VkDeviceAddress get_address();

 protected:
  void create_acceleration_structure(
      VkAccelerationStructureTypeKHR acceleration_structure_type,
      VkDeviceSize acceleration_structure_size);

  void build_acceleration_structure(
      const storage_buffer& scratch_buffer,
      VkDeviceAddress scratch_buffer_offset,
      const acceleration_structure_build_info& build_info) const;

 protected:
  unique_ptr<storage_buffer> m_acceleration_structure_buffer;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_ACCELERATION_STRUCTURE_H
