#ifndef VULKAN_ENVIRONMENT_H
#define VULKAN_ENVIRONMENT_H

#include "core/non_copyable.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"
#include "gla/vulkan/vulkan_texture_fwd.h"

namespace wunder::vulkan {
class descriptor_set_manager;
class base_renderer;
}

namespace wunder::vulkan {
struct vulkan_environment : public non_copyable {
 public:
  struct acceleration_data {
    unique_ptr<storage_buffer> m_buffer;
    float m_integral;
    float m_average_luminance;
  };

 public:
  unique_ptr<sampled_texture> m_image;
  acceleration_data m_acceleration_data;

  void add_descriptor_to(descriptor_set_manager& target);
};
}  // namespace wunder::vulkan

#endif  // VULKAN_ENVIRONMENT_H
