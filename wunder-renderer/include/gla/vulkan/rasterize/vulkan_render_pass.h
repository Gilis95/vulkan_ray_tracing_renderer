#ifndef WUNDER_VULKAN_RENDER_PASS_H
#define WUNDER_VULKAN_RENDER_PASS_H

#include <glad/vulkan.h>

namespace wunder::vulkan {
class render_pass {
 public:
  render_pass(VkFormat colour_format);
  ~render_pass();

 public:
  void initialize();
  void deallocate();

 public:
  void begin(VkFramebuffer framebuffer, VkExtent2D size);
  void end();

 public:
  VkRenderPass get_vulkan_render_pass() { return m_render_pass; }

 private:
  VkRenderPass m_render_pass;
  VkFormat m_colour_format;
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_RENDER_PASS_H
