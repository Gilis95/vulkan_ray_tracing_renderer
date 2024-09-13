#ifndef WUNDER_VULKAN_BUFFER_H
#define WUNDER_VULKAN_BUFFER_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include "core/non_copyable.h"

namespace wunder {
class vulkan_buffer : public non_copyable {
 public:
  vulkan_buffer() = default;
  virtual ~vulkan_buffer();

 public:
  vulkan_buffer(vulkan_buffer&&) noexcept ;
  vulkan_buffer& operator=(vulkan_buffer&&) noexcept ;

 public:
  [[nodiscard]] VkBuffer get_buffer() const { return m_vk_buffer; };
  [[nodiscard]] VkDeviceAddress get_address() const;

 protected:
  VkBuffer m_vk_buffer = VK_NULL_HANDLE;
  VmaAllocation m_allocation = VK_NULL_HANDLE;
};
}  // namespace wunder

#endif  // WUNDER_VULKAN_BUFFER_H
