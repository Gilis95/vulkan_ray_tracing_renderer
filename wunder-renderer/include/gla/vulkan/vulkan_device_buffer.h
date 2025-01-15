#ifndef WUNDER_VULKAN_DEVICE_BUFFER_H
#define WUNDER_VULKAN_DEVICE_BUFFER_H

#include <cstddef>
#include <utility>

#include "gla/vulkan/vulkan_buffer.h"

namespace wunder::vulkan {

template <typename base_buffer_type>
class device_buffer : public buffer<base_buffer_type> {
 public:
  device_buffer(
      descriptor_build_data descriptor_build_data,
      size_t data_size, VkBufferUsageFlags usage_flags);
  device_buffer(
      descriptor_build_data descriptor_build_data,
      const void* data, size_t data_size, VkBufferUsageFlags usage_flags);

  ~device_buffer() override;

 public:
  void update_data(void* data, size_t data_size) override;

 private:
  void allocate_device_buffer(size_t data_size, VkBufferUsageFlags usage_flags);

 private:
  [[nodiscard]] static std::pair<VkBuffer, VmaAllocation>
  allocate_cpu_staging_buffer(size_t data_size);
};

}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_BUFFER_H
