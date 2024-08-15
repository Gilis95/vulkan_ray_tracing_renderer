#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <glad/vulkan.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "core/wunder_memory.h"
#include "gla/renderer_api.h"

namespace wunder {

class vulkan_shader;

class vulkan_renderer : public renderer_api {
 public:
  ~vulkan_renderer() override;

 public:
  [[nodiscard]] const renderer_capabilities& get_capabilities() const override;

  void update(int dt) override;

 protected:
  void init_internal(const renderer_properties& properties) override;

 private:
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;  // Vulkan window surface
  std::unordered_map<VkShaderStageFlagBits,
                     std::vector<std::unique_ptr<vulkan_shader>>>
      m_shaders;
};
}  // namespace wunder
#endif /* VULKAN_RENDERER_H */
