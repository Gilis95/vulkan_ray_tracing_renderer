#ifndef WUNDER_VULKAN_DEVICE_BUFFER_H
#define WUNDER_VULKAN_DEVICE_BUFFER_H

#include <cstddef>
#include <utility>

#include "gla/vulkan/vulkan_buffer.h"

namespace wunder::vulkan {

class device_buffer : public buffer {
 public:
  device_buffer(size_t data_size, VkBufferUsageFlags usage_flags);
  device_buffer(const void* data, size_t data_size,
                       VkBufferUsageFlags usage_flags);

  ~device_buffer() override;

 private:
  void allocate_device_buffer(size_t data_size, VkBufferUsageFlags usage_flags);

 private:
  [[nodiscard]] static std::pair<VkBuffer, VmaAllocation>
  allocate_cpu_staging_buffer(size_t data_size);
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_BUFFER_H
