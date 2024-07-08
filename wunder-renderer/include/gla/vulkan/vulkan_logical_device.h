//
// Created by christian on 7/3/24.
//

#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_LOGICAL_DEVICE_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_LOGICAL_DEVICE_H_

#include <glad/vulkan.h>

#include <memory>
#include <vector>

namespace wunder {

class vulkan_physical_device;
class vulkan_command_pool;

// Represents a logical device
class vulkan_logical_device {
 public:
  vulkan_logical_device(vulkan_physical_device& vulkan_physical_device,
                        VkPhysicalDeviceFeatures enabled_features);
  ~vulkan_logical_device();
 public:
  [[nodiscard]] VkQueue get_graphics_queue() { return m_graphics_queue; }
  [[nodiscard]] VkQueue get_compute_queue() { return m_compute_queue; }
  [[nodiscard]] const vulkan_physical_device& get_physical_device() const {
    return m_physical_device;
  }
  [[nodiscard]] VkDevice get_vulkan_logical_device() const {
    return m_logical_device;
  }

  void flush_command_buffer(VkCommandBuffer commandBuffer);
  void flush_command_buffer(VkCommandBuffer commandBuffer, VkQueue queue);
 private:
  bool try_add_extension(std::vector<const char *> &device_extensions, const char *extension) const;

  void create_logical_device();
  void destroy();
 private:
  VkDevice m_logical_device = VK_NULL_HANDLE;
  VkQueue m_graphics_queue = VK_NULL_HANDLE;
  VkQueue m_compute_queue= VK_NULL_HANDLE;
  VkPhysicalDeviceFeatures m_enabled_features;

  vulkan_physical_device& m_physical_device;
  std::unique_ptr<vulkan_command_pool> m_command_pool;
};

}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_LOGICAL_DEVICE_H_
