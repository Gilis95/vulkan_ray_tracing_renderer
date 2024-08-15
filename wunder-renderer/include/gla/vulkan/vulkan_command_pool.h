#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_

#include <glad/vulkan.h>

namespace wunder {
class vulkan_device;

class vulkan_command_pool {
 public:
  explicit vulkan_command_pool();
  virtual ~vulkan_command_pool();

 public:
  VkCommandBuffer allocate_graphics_command_buffer(bool begin);
  VkCommandBuffer allocate_compute_command_buffer(bool begin);

  void flush_graphics_command_buffer();
  void flush_compute_command_buffer();

 public:
  [[nodiscard]] VkCommandPool get_graphics_command_pool() const {
    return m_graphics_command_pool;
  }

  [[nodiscard]] VkCommandPool get_compute_command_pool() const {
    return m_compute_command_pool;
  }

  [[nodiscard]] VkCommandBuffer get_current_graphics_command_buffer() const {
    return m_current_graphics_command_buffer;
  }

  [[nodiscard]] VkCommandBuffer get_current_compute_command_buffer() const {
    return m_current_compute_command_buffer;
  }

 private:
  VkCommandBuffer allocate_command_buffer(bool begin, VkCommandPool& out_pool,
                                          VkCommandBuffer& out_buffer);
  void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue);

 private:
  VkCommandPool m_graphics_command_pool = VK_NULL_HANDLE;
  VkCommandPool m_compute_command_pool = VK_NULL_HANDLE;

  VkCommandBuffer m_current_graphics_command_buffer = VK_NULL_HANDLE;
  VkCommandBuffer m_current_compute_command_buffer = VK_NULL_HANDLE;
};
}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_
