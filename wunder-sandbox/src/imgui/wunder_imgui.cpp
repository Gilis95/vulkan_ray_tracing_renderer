#include "imgui/wunder_imgui.h"

#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "core/wunder_logger.h"
#include "event/event_handler.h"
#include "event/event_handler.hpp"
#include "event/vulkan_events.h"
#include "gla/vulkan/rasterize/vulkan_render_pass.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "gla/vulkan/vulkan_renderer_context.h"
#include "window/glfw/glfw_window.h"
#include "window/window_factory.h"

namespace wunder {
wunder_imgui::wunder_imgui()
    : event_handler<wunder::event::vulkan::swap_chain_destroyed>() {}

wunder_imgui::~wunder_imgui() {};

void wunder_imgui::init() {
  auto& gla = wunder::vulkan::layer_abstraction_factory::instance();
  auto& vulkan_context = gla.get_vulkan_context();
  auto& physical_device = vulkan_context.mutable_physical_device();
  auto& device = vulkan_context.mutable_device();
  auto& swap_chain = gla.get_render_context().mutable_swap_chain();
  auto maybe_window =
      wunder::window_factory::instance().get_window().as<wunder::glfw_window>();

  if (!maybe_window.has_value()) {
    WUNDER_ERROR_TAG("Editor", "No window was found");
    return;
  }

  auto& window = maybe_window.value().get();

  m_render_pass = std::make_unique<vulkan::render_pass>(
      "imgui pass", VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE);

  std::vector<VkDescriptorPoolSize> poolSize{
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1}};
  VkDescriptorPoolCreateInfo poolInfo{
      VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
  poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  poolInfo.maxSets = 1000;
  poolInfo.poolSizeCount = 2;
  poolInfo.pPoolSizes = poolSize.data();
  vkCreateDescriptorPool(device.get_vulkan_logical_device(), &poolInfo, nullptr,
                         &m_imgui_desc_pool);

  // Setup Platform/Renderer back ends
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = vulkan_context.mutable_vulkan().get_instance();
  init_info.PhysicalDevice = physical_device.get_vulkan_physical_device();
  init_info.Device = device.get_vulkan_logical_device();
  init_info.QueueFamily = physical_device.get_queue_family_indices().Graphics;
  init_info.Queue = device.get_graphics_queue();
  init_info.PipelineCache = VK_NULL_HANDLE;
  init_info.DescriptorPool = m_imgui_desc_pool;
  init_info.RenderPass = m_render_pass->get_vulkan_render_pass();
  init_info.Subpass = 0;
  init_info.MinImageCount = 2;
  init_info.ImageCount = static_cast<uint32_t>(swap_chain.get_image_count());
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;  // <--- need argument?
  init_info.CheckVkResultFn = nullptr;
  init_info.Allocator = nullptr;

  init_info.UseDynamicRendering = VK_FALSE;
  init_info.PipelineRenderingCreateInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
  init_info.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
  init_info.PipelineRenderingCreateInfo.pColorAttachmentFormats =
      &swap_chain.get_colour_format();
  init_info.PipelineRenderingCreateInfo.depthAttachmentFormat =
      physical_device.get_depth_format();

  ImGui::CreateContext();

  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = nullptr;  // Avoiding the INI file
  io.LogFilename = nullptr;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard;            // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking

  ImGui_ImplVulkan_Init(&init_info);

  ImGui_ImplGlfw_InitForVulkan(window.mutable_window(), true);
}

void wunder_imgui::shutdown() {}

void wunder_imgui::update(wunder::time_unit dt) {
  auto& gla = wunder::vulkan::layer_abstraction_factory::instance();
  auto& swap_chain = gla.get_render_context().mutable_swap_chain();

  m_render_pass->begin();

  // Start the Dear ImGui frame
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  m_right_side_panel.update(dt);

  ImGui::Render();
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                  swap_chain.get_current_command_buffer());
  m_render_pass->end();
}

void wunder_imgui::on_event(
    const event::vulkan::swap_chain_destroyed&) /*override*/ {
  vulkan::context& context =
      vulkan::layer_abstraction_factory::instance().get_vulkan_context();
  auto* const device = context.mutable_device().get_vulkan_logical_device();

  ImGui_ImplVulkan_Shutdown();

  m_render_pass.reset();

  if (m_imgui_desc_pool) {
    vkDestroyDescriptorPool(device, m_imgui_desc_pool, VK_NULL_HANDLE);
  }
}

}  // namespace wunder