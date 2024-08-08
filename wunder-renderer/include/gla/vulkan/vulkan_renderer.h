#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <glad/vulkan.h>

#include <string>
#include <vector>

#include "gla/renderer_api.h"
#include "core/wunder_memory.h"

namespace wunder {
class vulkan;
class vulkan_physical_device;
class vulkan_logical_device;
class vulkan_command_pool;
struct vulkan_extensions;

class vulkan_renderer : public renderer_api {
 public:
  ~vulkan_renderer() override;

 protected:
  void init_internal(const renderer_properties& properties) override;

 private:
  void create_vulkan_instance(const renderer_properties& properties);
  void select_logical_device();
  void select_physical_device();

  void update(int dt) override;

  renderer_capabilities& get_capabilities() override;

  private:
  shared_ptr<vulkan> m_vulkan;
  unique_ptr<renderer_capabilities> m_renderer_capabilities;
  shared_ptr<vulkan_physical_device> m_physical_device;
  unique_ptr<vulkan_logical_device> m_logical_device;

  VkSurfaceKHR m_surface = VK_NULL_HANDLE;  // Vulkan window surface


};
}  // namespace wunder
#endif /* VULKAN_RENDERER_H */
