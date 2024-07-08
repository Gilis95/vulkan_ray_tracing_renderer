#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <glad/vulkan.h>

#include <memory>

#include "gla/renderer_api.h"

namespace wunder {
class vulkan_physical_device;
class vulkan_logical_device;
class vulkan_command_pool;

class vulkan_renderer : public renderer_api {
 public:
  ~vulkan_renderer() override;

 protected:
  void init_internal(const renderer_properties& properties) override;

 private:
  VkResult create_vulkan_instance(const renderer_properties& properties);
  VkResult try_add_validation_layer(VkInstanceCreateInfo& instance_create_info,
                                    const renderer_properties& properties) const;

  void select_logical_device();
  void select_physical_device();

 private:
  VkInstance m_vk_instance = VK_NULL_HANDLE;  // Vulkan library handle
  VkDebugUtilsMessengerEXT m_debug_messenger =
      VK_NULL_HANDLE;  // Vulkan debug output handle

  std::unique_ptr<vulkan_physical_device> m_physical_device;
  std::unique_ptr<vulkan_logical_device> m_logical_device;

  VkSurfaceKHR m_surface = VK_NULL_HANDLE;  // Vulkan window surface
};
}  // namespace wunder
#endif /* VULKAN_RENDERER_H */
