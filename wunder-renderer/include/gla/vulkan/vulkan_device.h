//
// Created by christian on 7/3/24.
//

#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_LOGICAL_DEVICE_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_LOGICAL_DEVICE_H_

#include <glad/vulkan.h>

#include <memory>
#include <vector>

#include "core/non_copyable.h"
#include "core/wunder_memory.h"

namespace wunder {

class vulkan_physical_device;
class vulkan_command_pool;
struct vulkan_extension_data;
struct physical_device_info;

// Represents a logical device
class vulkan_device : public non_copyable{
 public:
  explicit vulkan_device(VkPhysicalDeviceFeatures enabled_features);
  ~vulkan_device();

 public:
  void initialize();

  [[nodiscard]] VkDevice get_vulkan_logical_device() const {
    return m_logical_device;
  }

  vulkan_command_pool &get_command_pool() { return *m_command_pool; }
  [[nodiscard]] const vulkan_command_pool &get_command_pool() const {
    return *m_command_pool;
  }

 public:
  [[nodiscard]] VkQueue get_graphics_queue() { return m_graphics_queue; }
  [[nodiscard]] VkQueue get_compute_queue() { return m_compute_queue; }

 private:
  void create_extensions_list();
  void create_logical_device();

  static void append_used_device_features(
      physical_device_info &physical_device_info,
      const std::vector<void *> &used_features,
      VkDeviceCreateInfo &out_device_create_info);

  void destroy();

 private:
  VkDevice m_logical_device = VK_NULL_HANDLE;
  VkQueue m_graphics_queue = VK_NULL_HANDLE;
  VkQueue m_compute_queue = VK_NULL_HANDLE;

  std::vector<vulkan_extension_data> m_used_extensions;
  std::vector<vulkan_extension_data> m_requested_extensions;
  unique_ptr<vulkan_command_pool> m_command_pool;
};

}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_LOGICAL_DEVICE_H_
