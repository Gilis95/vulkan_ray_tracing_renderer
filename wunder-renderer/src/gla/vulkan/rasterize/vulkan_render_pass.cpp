#include "gla/vulkan/rasterize/vulkan_render_pass.h"

#include <glad/vulkan.h>

#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_physical_device.h"

namespace wunder::vulkan {

render_pass::render_pass(VkFormat colour_format)
    : m_render_pass(VK_NULL_HANDLE), m_colour_format(colour_format) {
  initialize();
}
render_pass::~render_pass() { deallocate(); }

void render_pass::initialize() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto& physical_device = vulkan_context.mutable_physical_device();
  auto vk_device = device.get_vulkan_logical_device();

  if (m_render_pass) {
    vkDestroyRenderPass(vk_device, m_render_pass, nullptr);
  }

  std::array<VkAttachmentDescription, 2> attachments{};
  // Color attachment
  attachments[0].format = m_colour_format;
  attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;

  // Depth attachment
  attachments[1].format = physical_device.get_depth_format();
  attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;

  // One color, one depth
  const VkAttachmentReference colorReference{
      0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
  const VkAttachmentReference depthReference{
      1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

  std::array<VkSubpassDependency, 1> subpass_dependencies{};
  // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all
  // commands executed outside of the actual renderpass)
  subpass_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
  subpass_dependencies[0].dstSubpass = 0;
  subpass_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  subpass_dependencies[0].dstStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  subpass_dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
  subpass_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  subpass_dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

  VkSubpassDescription subpass_description{};
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &colorReference;
  subpass_description.pDepthStencilAttachment = &depthReference;

  VkRenderPassCreateInfo render_pass_create_info{};
  render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_create_info.attachmentCount =
      static_cast<uint32_t>(attachments.size());
  render_pass_create_info.pAttachments = attachments.data();
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pSubpasses = &subpass_description;
  render_pass_create_info.dependencyCount =
      static_cast<uint32_t>(subpass_dependencies.size());
  render_pass_create_info.pDependencies = subpass_dependencies.data();

  VK_CHECK_RESULT(vkCreateRenderPass(vk_device, &render_pass_create_info,
                                     nullptr, &m_render_pass));
  set_debug_utils_object_name(vk_device, "Swapchain render pass",
                              m_render_pass);
}

void render_pass::deallocate() const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();

  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  vkDestroyRenderPass(vk_device, m_render_pass, nullptr);
}

void render_pass::begin(VkFramebuffer framebuffer, VkExtent2D size) const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();

  auto command_buffer =
      vulkan_context.mutable_swap_chain().get_current_command_buffer();

  std::array<VkClearValue, 2> clear_values{};
  clear_values[0].color = {{0.0f, 0.0f, 0.0f, 0.0f}};
  clear_values[1].depthStencil = {1.0f, 0};

  VkRenderPassBeginInfo render_pass_begin_info{};
  render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_begin_info.clearValueCount = 2;
  render_pass_begin_info.pClearValues = clear_values.data();
  render_pass_begin_info.renderPass = m_render_pass;
  render_pass_begin_info.framebuffer = framebuffer;
  render_pass_begin_info.renderArea = {{}, size};

  vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void render_pass::end() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto command_buffer =
      vulkan_context.mutable_swap_chain().get_current_command_buffer();

  vkCmdEndRenderPass(command_buffer);
}
}  // namespace wunder::vulkan