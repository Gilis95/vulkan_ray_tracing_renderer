#ifndef WUNDER_VULKAN_RENDER_PASS_H
#define WUNDER_VULKAN_RENDER_PASS_H

#include <glad/vulkan.h>

namespace wunder::vulkan {
class render_pass {
 public:
  explicit render_pass();
  ~render_pass();

 public:
  void init(VkAttachmentLoadOp load_op);
 private:
  void deallocate() const;

 public:
  void push_load_operation(VkAttachmentLoadOp load_op);

  void begin();
  void end();
  void create_frame_buffer_for_each_queue_element();

 private:
  void fill_attachment_info(VkAttachmentLoadOp load_op);

 private:
  VkRenderingAttachmentInfoKHR m_color_attachment_info {};
  VkRenderingAttachmentInfoKHR m_depth_attachment_info {};
  VkRenderingInfoKHR m_render_info {};
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_RENDER_PASS_H
