#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_

#include <glad/vulkan.h>

namespace wunder {
class vulkan_logical_device;

class vulkan_command_pool {
 public:
  explicit vulkan_command_pool(vulkan_logical_device &logical_device);
  virtual ~vulkan_command_pool();

  VkCommandBuffer allocate_command_buffer(bool begin, bool compute = false);
  void flush_command_buffer(VkCommandBuffer commandBuffer);
  void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue);

  [[nodiscard]] VkCommandPool get_graphics_command_pool() const { return m_graphics_command_pool; }
  [[nodiscard]] VkCommandPool get_compute_command_pool() const { return m_compute_command_pool; }
 private:
  VkCommandPool m_graphics_command_pool = VK_NULL_HANDLE;
  VkCommandPool m_compute_command_pool= VK_NULL_HANDLE;
  vulkan_logical_device &m_logical_device;
};
}
#endif //WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_
