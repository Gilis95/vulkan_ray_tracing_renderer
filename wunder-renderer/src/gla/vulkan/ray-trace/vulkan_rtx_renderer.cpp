#include "include/gla/vulkan/ray-trace/vulkan_rtx_renderer.h"

#include <optional>

#include "camera/camera.h"
#include "core/project.h"
#include "core/services_factory.h"
#include "event/event_handler.hpp"
#include "event/scene_events.h"
#include "gla/renderer_capabilities .h"
#include "gla/vulkan/rasterize/vulkan_rasterize_renderer.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/scene/vulkan_scene.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_shader.h"
#include "gla/vulkan/vulkan_shader_binding_table.h"
#include "gla/vulkan/vulkan_texture.h"
#include "glad/vulkan.h"
#include "include/gla/vulkan/ray-trace/vulkan_rtx_pipeline.h"
#include "resources/shaders/host_device.h"
#include "scene/scene_manager.h"

namespace wunder::vulkan {
rtx_renderer::rtx_renderer(const renderer_properties &renderer_properties)
    : event_handler<wunder::event::scene_activated>(),
      m_renderer_properties(renderer_properties),
      m_have_active_scene(false) {}

rtx_renderer::~rtx_renderer() {
  if (m_rtx_pipeline.get()) {
    AssertLogUnless(m_rtx_pipeline.release());
  }
  if (m_shader_binding_table.get()) {
    AssertLogUnless(m_shader_binding_table.release());
  }
}

void rtx_renderer::init_internal(const renderer_properties &properties) {
  m_rtx_pipeline = std::make_unique<rtx_pipeline>();
  m_shader_binding_table = std::make_unique<shader_binding_table>();
  m_rasterize_renderer = std::make_unique<rasterize_renderer>(properties);

  m_state = std::make_unique<RtxState>();
  m_state->maxDepth = 3;

  initialize_shaders();

  m_rtx_pipeline->initialize_pipeline(m_shaders);
  m_shader_binding_table->initialize(*m_rtx_pipeline);

  m_rasterize_renderer->initialize();
}

vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
rtx_renderer::get_shaders_for_compilation() {
  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
      shaders_to_compile;
  shaders_to_compile[VK_SHADER_STAGE_RAYGEN_BIT_KHR] =
      std::vector(std::initializer_list{shader_to_compile{
          .m_shader_path = "wunder-renderer/resources/shaders/pathtrace.rgen",
          .m_on_successful_compile =
              std::bind(&rtx_renderer::create_descriptor_manager, this,
                        std::placeholders::_1),
          .m_optional = false}});
  shaders_to_compile[VK_SHADER_STAGE_ANY_HIT_BIT_KHR] =
      std::vector(std::initializer_list{shader_to_compile{
          .m_shader_path = "wunder-renderer/resources/shaders/pathtrace.rahit",
          .m_on_successful_compile = nullptr,
          .m_optional = false}});
  shaders_to_compile[VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR] =
      std::vector(std::initializer_list{shader_to_compile{
          .m_shader_path = "wunder-renderer/resources/shaders/pathtrace.rchit",
          .m_on_successful_compile = nullptr,
          .m_optional = false}});
  shaders_to_compile[VK_SHADER_STAGE_MISS_BIT_KHR] =
      std::vector(std::initializer_list{
          shader_to_compile{
              .m_shader_path =
                  "wunder-renderer/resources/shaders/pathtrace.rmiss",
              .m_on_successful_compile = nullptr,
              .m_optional = false},
          shader_to_compile{
              .m_shader_path =
                  "wunder-renderer/resources/shaders/pathtraceShadow.rmiss",
              .m_on_successful_compile = nullptr,
              .m_optional = false}});
  return shaders_to_compile;
}

void rtx_renderer::create_descriptor_manager(const shader &shader) {
  m_descriptor_set_manager = std::make_unique<descriptor_set_manager>();
  m_descriptor_set_manager->initialize(shader);

  m_rtx_pipeline->initialize_pipeline_layout(shader);
}

void rtx_renderer::update(time_unit dt) /*override*/
{
  ReturnUnless(m_have_active_scene);
  auto &vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto &swap_chain = vulkan_context.mutable_swap_chain();


  AssertReturnUnless(swap_chain.acquire().has_value(), );

  swap_chain.begin_command_buffer();
  auto graphic_command_buffer =
    swap_chain.get_current_command_buffer();

  m_rtx_pipeline->bind();
  m_descriptor_set_manager->bind(*m_rtx_pipeline);

  vkCmdPushConstants(
      graphic_command_buffer, m_rtx_pipeline->get_vulkan_pipeline_layout(),
      VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR |
          VK_SHADER_STAGE_MISS_BIT_KHR,
      0, sizeof(RtxState), m_state.get());

  //  auto& regions = m_sbtWrapper.getRegions();
  VkStridedDeviceAddressRegionKHR raygen_address =
      m_shader_binding_table->get_stage_address(
          shader_binding_table::shader_stage_type::raygen);
  VkStridedDeviceAddressRegionKHR miss_address =
      m_shader_binding_table->get_stage_address(
          shader_binding_table::shader_stage_type::miss);
  VkStridedDeviceAddressRegionKHR hit_address =
      m_shader_binding_table->get_stage_address(
          shader_binding_table::shader_stage_type::hit);
  VkStridedDeviceAddressRegionKHR callable_address =
      m_shader_binding_table->get_stage_address(
          shader_binding_table::shader_stage_type::callable);

  vkCmdTraceRaysKHR(graphic_command_buffer, &raygen_address, &miss_address,
                    &hit_address, &callable_address,
                    m_renderer_properties.m_width,
                    m_renderer_properties.m_height, 3);
  ++m_state->frame;

  swap_chain.begin_render_pass();
  m_rasterize_renderer->draw_frame();
  swap_chain.end_render_pass();

  swap_chain.flush_current_command_buffer();
}

void rtx_renderer::on_event(
    const wunder::event::scene_activated &scene_activated_event) {
  auto api_scene = project::instance().get_scene_manager().mutable_api_scene(
      scene_activated_event.m_id);
  AssertReturnUnless(api_scene.has_value());

  m_descriptor_set_manager->clear_resources();

  api_scene->get().add_descriptor_to(*this);
  service_factory::instance().get_camera().bind(*this);
  m_rasterize_renderer->get_output_image().add_descriptor_to(*this);

  m_descriptor_set_manager->bake();

  m_have_active_scene = true;
}

const renderer_capabilities &rtx_renderer::get_capabilities() const /*override*/
{
  static renderer_capabilities s_empty;
  return layer_abstraction_factory::instance()
      .get_vulkan_context()
      .get_capabilities();
}
}  // namespace wunder::vulkan