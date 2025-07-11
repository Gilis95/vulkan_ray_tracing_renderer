#include "gla/vulkan/rasterize/vulkan_render_pass.h"

#include <glad/vulkan.h>

#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_renderer_context.h"

namespace wunder::vulkan {

render_pass::render_pass() = default;
render_pass::~render_pass() { deallocate(); }

void render_pass::init(VkAttachmentLoadOp load_op) {
  fill_attachment_info(load_op);
}

void render_pass::deallocate() const {}

void render_pass::push_load_operation(VkAttachmentLoadOp load_op) {
  m_color_attachment_info.loadOp = load_op;
  m_depth_attachment_info.loadOp = load_op;
}

void render_pass::begin() {
  auto& swap_chain = layer_abstraction_factory::instance()
                         .get_render_context()
                         .mutable_swap_chain();

  auto command_buffer = swap_chain.get_current_command_buffer();

  m_color_attachment_info.imageView =
      swap_chain.m_queue_elements[swap_chain.get_current_queue_element()]
          .m_image_view;

  vkCmdBeginRendering(command_buffer, &m_render_info);
}

void render_pass::end() {
  auto command_buffer = layer_abstraction_factory::instance()
                            .get_render_context()
                            .mutable_swap_chain()
                            .get_current_command_buffer();

  vkCmdEndRendering(command_buffer);
}

void render_pass::fill_attachment_info(VkAttachmentLoadOp load_op) {
  auto& swap_chain = layer_abstraction_factory::instance()
                         .get_render_context()
                         .mutable_swap_chain();

  m_color_attachment_info.sType =
      VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
  m_color_attachment_info.pNext = VK_NULL_HANDLE;
  m_color_attachment_info.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  m_color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
  m_color_attachment_info.resolveImageView = VK_NULL_HANDLE;
  m_color_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  m_color_attachment_info.loadOp = load_op;
  m_color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  m_color_attachment_info.clearValue = {.color = {{0.f, 0.f, 0.f, 1.f}}};

  m_depth_attachment_info.sType =
      VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
  m_depth_attachment_info.pNext = VK_NULL_HANDLE;
  m_depth_attachment_info.imageView = swap_chain.m_depth_view;
  m_depth_attachment_info.imageLayout =
      VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
  m_depth_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
  m_depth_attachment_info.resolveImageView = VK_NULL_HANDLE;
  m_depth_attachment_info.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  m_depth_attachment_info.loadOp = load_op;
  m_depth_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  m_depth_attachment_info.clearValue = {.depthStencil = {1.f, 0}};

  m_render_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
  m_render_info.pNext = VK_NULL_HANDLE;
  m_render_info.flags = 0;
  m_render_info.renderArea = {{}, {swap_chain.m_width, swap_chain.m_height}};
  m_render_info.layerCount = 1;
  m_render_info.viewMask = 0;
  m_render_info.colorAttachmentCount = 1;
  m_render_info.pColorAttachments = &m_color_attachment_info;
  m_render_info.pDepthAttachment = &m_depth_attachment_info;
  m_render_info.pStencilAttachment =
      VK_NULL_HANDLE;  // or &stencilAttachment if needed
                       //
}

}  // namespace wunder::vulkan
