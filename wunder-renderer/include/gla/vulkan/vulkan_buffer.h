#ifndef WUNDER_VULKAN_BUFFER_H
#define WUNDER_VULKAN_BUFFER_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include "core/non_copyable.h"
#include "gla/vulkan/vulkan_types.h"

namespace wunder::vulkan {
class renderer;

class buffer : public non_copyable,
               public vulkan::shader_resource::instance::storage_buffers {
 public:
  struct descriptor_build_data {
    bool m_enabled = false;
    std::string m_descriptor_name;
  };

 public:
  buffer(descriptor_build_data descriptor_build_data);
  virtual ~buffer();

 public:
  buffer(buffer&&) noexcept;
  buffer& operator=(buffer&&) noexcept;

 public:
  void bind(renderer& renderer) override;

  [[nodiscard]] VkBuffer get_buffer() const { return m_vk_buffer; };
  [[nodiscard]] VkDeviceAddress get_address() const;

 protected:
  descriptor_build_data m_descriptor_build_data;
  VkBuffer m_vk_buffer = VK_NULL_HANDLE;
  VmaAllocation m_allocation = VK_NULL_HANDLE;
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_BUFFER_H
