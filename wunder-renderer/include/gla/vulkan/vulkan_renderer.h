#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <glad/vulkan.h>

#include "gla/renderer_api.h"

namespace wunder {
class vulkan_renderer : public renderer_api {
 public:
  ~vulkan_renderer() override;

 protected:
  void init_internal(const renderer_properties &properties) override;

 private:
  VkResult create_vulkan_instance();

  VkResult select_gpu();

  VkResult select_queue_family();

  VkResult create_vulkan_logical_device();

 private:
  VkInstance m_vk_instance = VK_NULL_HANDLE;  // Vulkan library handle
  VkDebugUtilsMessengerEXT m_debug_messenger =
      VK_NULL_HANDLE;  // Vulkan debug output handle

  VkPhysicalDevice m_physical_device =
      VK_NULL_HANDLE;                  // chosen physical device
  VkDevice m_device = VK_NULL_HANDLE;  // Vulkan device for commands

  int m_selected_queue_family;
  VkQueue m_queue = VK_NULL_HANDLE;
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;  // Vulkan window surface
  VkResult create_descriptor_pool();
};
}  // namespace wunder
#endif /* VULKAN_RENDERER_H */
