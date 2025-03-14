
#ifndef VULKAN_ACCELERATION_STRUCTURE_BUILDER_H
#define VULKAN_ACCELERATION_STRUCTURE_BUILDER_H

#include "core/wunder_macros.h"
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_build_info.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"

namespace wunder::vulkan {

template <derived<acceleration_structure_build_info> build_info_type>
class vulkan_acceleration_structure_builder {
 public:
  vulkan_acceleration_structure_builder();
  virtual ~vulkan_acceleration_structure_builder() = default;

 protected:
  void create_scratch_buffer(std::uint32_t scratch_buffer_size);
  void build_acceleration_structure();

  virtual const std::vector<build_info_type>& get_build_infos() const = 0;

 protected:
  unique_ptr<storage_buffer> m_scratch_buffer;
};
}  // namespace wunder::vulkan
#endif  // VULKAN_ACCELERATION_STRUCTURE_BUILDER_H
