#include "gla/vulkan/rasterize/vulkan_render_pass.h"

#include <glad/vulkan.h>

#include <utility>

#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "gla/vulkan/vulkan_renderer_context.h"

namespace wunder::vulkan {

render_pass::render_pass(std::string name, VkAttachmentLoadOp load_op)
    : m_name(std::move(name)),
      m_load_op(load_op),
      m_render_pass(VK_NULL_HANDLE) {
  initialize();
}
render_pass::~render_pass() { deallocate(); }

void render_pass::initialize() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  if (m_render_pass) {
    vkDestroyRenderPass(vk_device, m_render_pass, nullptr);
  }

  VkRenderPassCreateInfo render_pass_create_info{};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

  fill_attachment_info(render_pass_create_info);
  fill_subpass_info(render_pass_create_info);

  VK_CHECK_RESULT(vkCreateRenderPass(vk_device, &render_pass_create_info,
                                     nullptr, &m_render_pass));
  set_debug_utils_object_name(vk_device, m_name, m_render_pass);

  create_frame_buffer_for_each_queue_element();
}

void render_pass::deallocate() const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();

  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  vkDestroyRenderPass(vk_device, m_render_pass, nullptr);

  for (auto framebuffer : m_framebuffers) {
    ContinueIf(framebuffer == VK_NULL_HANDLE);

    vkDestroyFramebuffer(vk_device, framebuffer, VK_NULL_HANDLE);
  }
}

void render_pass::begin() const {
  auto& swap_chain = layer_abstraction_factory::instance()
                         .get_render_context()
                         .mutable_swap_chain();
  auto command_buffer = swap_chain.get_current_command_buffer();

  std::array<VkClearValue, 2> clear_values{};
  clear_values[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
  clear_values[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo render_pass_begin_info{};
  render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_begin_info.clearValueCount = 2;
  render_pass_begin_info.pClearValues = clear_values.data();
  render_pass_begin_info.renderPass = m_render_pass;
  render_pass_begin_info.framebuffer =
      m_framebuffers[swap_chain.get_current_queue_element()];
  render_pass_begin_info.renderArea = {
      {}, {swap_chain.m_width, swap_chain.m_height}};

  vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void render_pass::end() {
  auto command_buffer = layer_abstraction_factory::instance()
                            .get_render_context()
                            .mutable_swap_chain()
                            .get_current_command_buffer();

  vkCmdEndRenderPass(command_buffer);
}

void render_pass::create_frame_buffer_for_each_queue_element() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();
  auto& render_context =
      layer_abstraction_factory::instance().get_render_context();
  auto& swap_chain = render_context.mutable_swap_chain();

  std::array<VkImageView, 2> attachments{};
  attachments[1] = swap_chain.m_depth_view;

  VkFramebufferCreateInfo frameBufferCreateInfo = {};
  frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  frameBufferCreateInfo.renderPass = m_render_pass;
  frameBufferCreateInfo.attachmentCount = 2;
  frameBufferCreateInfo.width = swap_chain.m_width;
  frameBufferCreateInfo.height = swap_chain.m_height;
  frameBufferCreateInfo.layers = 1;
  frameBufferCreateInfo.pAttachments = attachments.data();

  m_framebuffers.resize(swap_chain.m_queue_elements.size());
  std::ranges::fill(m_framebuffers, VK_NULL_HANDLE);

  for (size_t i = 0; i < swap_chain.m_queue_elements.size(); i++) {
    auto& queue_element = swap_chain.m_queue_elements[i];
    attachments[0] = queue_element.m_image_view;

    VK_CHECK_RESULT(vkCreateFramebuffer(vk_device, &frameBufferCreateInfo,
                                        nullptr, &m_framebuffers[i]));
    set_debug_utils_object_name(
        vk_device,
        std::format("Swapchain framebuffer (Frame in flight: {})", i),
        m_framebuffers[i]);
  }
}

void render_pass::fill_attachment_info(
    VkRenderPassCreateInfo& render_pass_create_info) {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.mutable_physical_device();
  auto& render_context =
      layer_abstraction_factory::instance().get_render_context();
  auto& swap_chain = render_context.mutable_swap_chain();

  m_attachments = {};
  // Color attachment
  m_attachments[0].format = swap_chain.m_colour_format;
  m_attachments[0].loadOp = m_load_op;
  m_attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  m_attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;

  // Depth attachment
  m_attachments[1].format = physical_device.get_depth_format();
  m_attachments[1].loadOp = m_load_op;
  m_attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  m_attachments[1].finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  m_attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;

  render_pass_create_info.attachmentCount =
      static_cast<uint32_t>(m_attachments.size());
  render_pass_create_info.pAttachments = m_attachments.data();
}

void render_pass::fill_subpass_info(
    VkRenderPassCreateInfo& render_pass_create_info) {
  // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all
  // commands executed outside of the actual renderpass)
  m_subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  m_subpass_dependencies[0].dstSubpass = 0;
  m_subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  m_subpass_dependencies[0].dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  m_subpass_dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  m_subpass_dependencies[0].dstAccessMask =
      VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  m_subpass_dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  m_subpass_description = {};
  m_subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  m_subpass_description.colorAttachmentCount = 1;
  m_subpass_description.pColorAttachments = &m_color_reference;
  m_subpass_description.pDepthStencilAttachment = &m_depth_reference;

  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pSubpasses = &m_subpass_description;
  render_pass_create_info.dependencyCount =
      static_cast<uint32_t>(m_subpass_dependencies.size());
  render_pass_create_info.pDependencies = m_subpass_dependencies.data();
}
}  // namespace wunder::vulkan