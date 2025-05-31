#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_

#include <glad/vulkan.h>

#include <string>

#include "core/non_copyable.h"

namespace wunder::vulkan {
class device;

class command_pool : public non_copyable {
 public:
  explicit command_pool();
  virtual ~command_pool();

 public:
  [[nodiscard]] VkCommandBuffer get_current_graphics_command_buffer();
  [[nodiscard]] VkCommandBuffer get_current_compute_command_buffer();

  void flush_graphics_command_buffer();
  void flush_compute_command_buffer();

 private:
  [[nodiscard]] VkCommandPool get_graphics_command_pool() const {
    return m_graphics_command_pool;
  }

  [[nodiscard]] VkCommandPool get_compute_command_pool() const {
    return m_compute_command_pool;
  }

 private:
  VkCommandBuffer allocate_graphics_command_buffer(bool begin);
  VkCommandBuffer allocate_compute_command_buffer(bool begin);

  VkCommandBuffer allocate_command_buffer(bool begin, VkCommandPool& out_pool,
                                          VkCommandBuffer& out_buffer, std::string name);
  void flush_command_buffer(VkCommandBuffer& command_buffer,
                            VkCommandPool source_pool, VkQueue queue);

 private:
  VkCommandPool m_graphics_command_pool = VK_NULL_HANDLE;
  VkCommandPool m_compute_command_pool = VK_NULL_HANDLE;

  VkCommandBuffer m_current_graphics_command_buffer = VK_NULL_HANDLE;
  VkCommandBuffer m_current_compute_command_buffer = VK_NULL_HANDLE;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_COMMAND_POOL_H_
