#include "include/gla/vulkan/ray-trace/vulkan_rtx_renderer.h"

#include <optional>

#include "camera/camera.h"
#include "core/project.h"
#include "core/services_factory.h"
#include "core/wunder_features.h"
#include "event/event_handler.hpp"
#include "event/scene_events.h"
#include "gla/renderer_capabilities .h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/rasterize/vulkan_rasterize_renderer.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/scene/vulkan_environment.h"
#include "gla/vulkan/scene/vulkan_scene.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "gla/vulkan/vulkan_renderer_context.h"
#include "gla/vulkan/vulkan_shader.h"
#include "gla/vulkan/vulkan_shader_binding_table.h"
#include "gla/vulkan/vulkan_texture.h"
#include "glad/vulkan.h"
#include "include/gla/vulkan/ray-trace/vulkan_rtx_pipeline.h"
#include "resources/shaders/host_device.h"
#include "scene/scene_manager.h"

namespace wunder::vulkan {
rtx_renderer::rtx_renderer(const renderer_properties &properties) :m_state(std::make_unique<RtxState>()) {
  m_state->maxDepth = 10;
  m_state->maxSamples = 7;
  m_state->fireflyClampThreshold = 0.f;
  m_state->hdrMultiplier = 1.7f;
  m_state->size = {properties.m_width, properties.m_height};
  m_state->minHeatmap = 0;
  m_state->maxHeatmap = 65000;
  m_state->debugging_mode = DebugMode::eNoDebug;
}

rtx_renderer::~rtx_renderer() = default;

void rtx_renderer::shutdown_internal() /*override*/ {
  if (m_rtx_pipeline.get()) {
    m_rtx_pipeline.reset();
  }

  if (m_shader_binding_table.get()) {
    m_shader_binding_table.reset();
  }

  if (m_state) {
    m_state.reset();
  }
}

void rtx_renderer::init_internal(scene_id scene_id) {
  auto api_scene = project::instance().get_scene_manager().mutable_api_scene(
        scene_id);
  AssertReturnUnless(api_scene.has_value());
  auto &scene = api_scene->get();
  const auto &environment_texture = scene.get_environment_texture();

  auto &renderer_context =
      layer_abstraction_factory::instance().get_render_context();
  auto &renderer_properties = renderer_context.get_renderer_properties();
  auto &m_rasterize_renderer = renderer_context.mutable_rasterize_renderer();

  auto &camera = service_factory::instance().get_camera();
  camera.set_window_size(renderer_properties.m_width,
                         renderer_properties.m_height);

  m_descriptor_set_manager->clear_resources();
  scene.collect_descriptors(*m_descriptor_set_manager);
  camera.collect_descriptors(*m_descriptor_set_manager);
  m_rasterize_renderer.get_output_image().add_descriptor_to(
      *m_descriptor_set_manager);

  AssertReturnIf(m_descriptor_set_manager->build() !=
                     descriptor_set_manager::build_error_code::SUCCESS, );

  m_rtx_pipeline =
      std::move(rtx_pipeline::create(*m_descriptor_set_manager, m_shaders));
  AssertReturnUnless(m_rtx_pipeline);

  m_shader_binding_table = shader_binding_table::create(*m_rtx_pipeline);

  m_state->frame = 0;
  m_state->fireflyClampThreshold =
      environment_texture.m_acceleration_data.m_integral;
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
  m_descriptor_set_manager = std::make_unique<descriptor_set_manager>(
      shader.get_shader_reflection_data());
}

void rtx_renderer::update(time_unit /*dt*/) /*override*/
{
  auto &renderer_context =
      layer_abstraction_factory::instance().get_render_context();
  auto &m_renderer_properties = renderer_context.get_renderer_properties();

  auto graphic_command_buffer =
      renderer_context.mutable_swap_chain().get_current_command_buffer();

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
                    m_renderer_properties.m_height, 1);

  log_current_sate_frame();
  ++m_state->frame;
}

void rtx_renderer::reset_frames() {
  m_state->frame = 0;
}

void rtx_renderer::on_event(const wunder::event::camera_moved &) /*override*/ {
  reset_frames();
}

void rtx_renderer::log_current_sate_frame() {
#if PRINT_STATE_FRAME
  WUNDER_TRACE_TAG("Rtx Renderer", "Frame {0}", m_state->frame);
#endif
}

}  // namespace wunder::vulkan