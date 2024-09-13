#ifndef WUNDER_VULKAN_DEVICE_BUFFER_H
#define WUNDER_VULKAN_DEVICE_BUFFER_H

#include <cstddef>
#include <utility>

#include "gla/vulkan/vulkan_buffer.h"

namespace wunder {
class vulkan_device_buffer : public vulkan_buffer {
 public:
  vulkan_device_buffer(size_t data_size,
                       VkBufferUsageFlags usage_flags);
  vulkan_device_buffer(const void* data, size_t data_size,
                       VkBufferUsageFlags usage_flags);

  ~vulkan_device_buffer() override;
 private:
  void allocate_device_buffer(size_t data_size, VkBufferUsageFlags usage_flags);
 private:
  [[nodiscard]] static std::pair<VkBuffer,VmaAllocation>  allocate_cpu_staging_buffer(
      size_t data_size);
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_BUFFER_H
