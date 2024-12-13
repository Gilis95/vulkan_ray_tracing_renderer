#ifndef WUNDER_VULKAN_BUFFER_H
#define WUNDER_VULKAN_BUFFER_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include "core/non_copyable.h"

namespace wunder::vulkan {
class renderer;

class buffer : public non_copyable {
 public:
  buffer() = default;
  virtual ~buffer();

 public:
  buffer(buffer&&) noexcept;
  buffer& operator=(buffer&&) noexcept;

 public:
  void bind(renderer& renderer);

  [[nodiscard]] VkBuffer get_buffer() const { return m_vk_buffer; };
  [[nodiscard]] VkDeviceAddress get_address() const;

 protected:
  VkBuffer m_vk_buffer = VK_NULL_HANDLE;
  VmaAllocation m_allocation = VK_NULL_HANDLE;
};
} // namespace wunder::vulkan


#endif  // WUNDER_VULKAN_BUFFER_H
