#ifndef WUNDER_VULKAN_RENDER_PASS_H
#define WUNDER_VULKAN_RENDER_PASS_H

#include <glad/vulkan.h>

#include <array>
#include <string>
#include <vector>

namespace wunder::vulkan {
class render_pass {
 public:
  render_pass(std::string name, VkAttachmentLoadOp load_op);
  ~render_pass();

 private:
  void initialize();
  void deallocate() const;

 public:
  void begin() const;
  void end();
  void create_frame_buffer_for_each_queue_element();

 public:
  VkRenderPass get_vulkan_render_pass() { return m_render_pass; }

 private:
  void fill_attachment_info(VkRenderPassCreateInfo& render_pass_create_info);
  void fill_subpass_info(VkRenderPassCreateInfo& render_pass_create_info);

 private:
  std::string m_name;
  VkAttachmentLoadOp m_load_op;
  VkRenderPass m_render_pass;
  std::vector<VkFramebuffer> m_framebuffers;

  std::array<VkAttachmentDescription, 2> m_attachments{};
  std::array<VkSubpassDependency, 1> m_subpass_dependencies{};
  VkSubpassDescription m_subpass_description{};

  // One color, one depth
  const VkAttachmentReference m_color_reference{
    0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  const VkAttachmentReference m_depth_reference{
    1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_RENDER_PASS_H
