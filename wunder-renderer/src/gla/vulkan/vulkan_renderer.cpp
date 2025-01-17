#include "gla/vulkan/vulkan_renderer.h"

#include <glad/vulkan.h>

#include <optional>

#include "camera/camera.h"
#include "core/project.h"
#include "core/services_factory.h"
#include "event/event_handler.hpp"
#include "event/scene_events.h"
#include "gla/renderer_capabilities .h"
#include "gla/vulkan/scene/vulkan_scene.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_pipeline.h"
#include "gla/vulkan/vulkan_shader.h"
#include "gla/vulkan/vulkan_shader_binding_table.h"
#include "resources/shaders/host_device.h"
#include "scene/scene_manager.h"

namespace wunder::vulkan {
renderer::renderer() : event_handler<wunder::event::scene_activated>() , m_have_active_scene(false) {}

renderer::~renderer() {
  if (m_pipeline.get()) {
    AssertLogUnless(m_pipeline.release());
  }
  if (m_shader_binding_table.get()) {
    AssertLogUnless(m_shader_binding_table.release());
  }
}

void renderer::init_internal(const renderer_properties &properties) {
  m_pipeline = std::make_unique<pipeline>();
  m_shader_binding_table = std::make_unique<shader_binding_table>();
  m_state = std::make_unique<RtxState>();
  m_state->maxDepth = 3;

  for (auto &[shader_type, shaders_compile_data] :
       get_shaders_for_compilation()) {
    auto &shaders_of_type = m_shaders[shader_type];

    for (auto &shader_compile_data : shaders_compile_data) {
      auto maybe_shader =
          shader::create(shader_compile_data.m_shader_path, shader_type);
      if (maybe_shader.has_value()) {
        auto &shader =
            shaders_of_type.emplace_back(std::move(maybe_shader.value()));

        ContinueUnless(shader_compile_data.m_on_successful_compile);
        shader_compile_data.m_on_successful_compile(*shader);
        continue;
      }

      WUNDER_ERROR_TAG("Renderer",
                       "Failed to compile {0} of type {1}. Error:  {2}",
                       shader_compile_data.m_shader_path.string(),
                       static_cast<int>(shader_type),
                       static_cast<int>(maybe_shader.error()));
      ContinueIf(shader_compile_data.m_optional);
      CRASH;
    }
  }

  m_pipeline->create_pipeline(m_shaders);
  m_shader_binding_table->initialize(*m_pipeline);
}

vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
renderer::get_shaders_for_compilation() {
  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
      shaders_to_compile;
  shaders_to_compile[VkShaderStageFlagBits::VK_SHADER_STAGE_RAYGEN_BIT_KHR] =
      std::vector<shader_to_compile>(
          std::initializer_list<shader_to_compile>{shader_to_compile{
              .m_shader_path =
                  "wunder-renderer/resources/shaders/pathtrace.rgen",
              .m_on_successful_compile =
                  std::bind(&renderer::create_descriptor_manager, this,
                            std::placeholders::_1),
              .m_optional = false}});
  shaders_to_compile[VkShaderStageFlagBits::VK_SHADER_STAGE_ANY_HIT_BIT_KHR] =
      std::vector<shader_to_compile>(
          std::initializer_list<shader_to_compile>{shader_to_compile{
              .m_shader_path =
                  "wunder-renderer/resources/shaders/pathtrace.rahit",
              .m_on_successful_compile = nullptr,
              .m_optional = false}});
  shaders_to_compile
      [VkShaderStageFlagBits::VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR] =
          std::vector<shader_to_compile>(
              std::initializer_list<shader_to_compile>{shader_to_compile{
                  .m_shader_path =
                      "wunder-renderer/resources/shaders/pathtrace.rchit",
                  .m_on_successful_compile = nullptr,
                  .m_optional = false}});
  shaders_to_compile[VkShaderStageFlagBits::VK_SHADER_STAGE_MISS_BIT_KHR] =
      std::vector<shader_to_compile>(std::initializer_list<shader_to_compile>{
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

void renderer::create_descriptor_manager(const shader &shader) {
  m_descriptor_set_manager = std::make_unique<descriptor_set_manager>();
  m_descriptor_set_manager->initialize(shader);

  m_pipeline->create_pipeline_layout(shader);
}

void renderer::update(time_unit dt) /*override*/
{
  ReturnUnless(m_have_active_scene);
  command_pool &command_pool = layer_abstraction_factory::instance()
                                   .get_vulkan_context()
                                   .get_device()
                                   .get_command_pool();
  auto graphic_command_buffer =
      command_pool.get_current_graphics_command_buffer();


  m_pipeline->bind();
  m_descriptor_set_manager->bind(*m_pipeline);

  vkCmdPushConstants(
      graphic_command_buffer, m_pipeline->get_vulkan_pipeline_layout(),
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
                    &hit_address, &callable_address, 100, 100, 3);
  ++m_state->frame;
  
  command_pool.flush_graphics_command_buffer();
}

void renderer::on_event(
    const wunder::event::scene_activated &scene_activated_event) {
  auto api_scene = project::instance().get_scene_manager().mutable_api_scene(
      scene_activated_event.m_id);

  AssertReturnUnless(api_scene.has_value());
  api_scene->get().bind(*this);
  service_factory::instance().get_camera().bind(*this);

  m_descriptor_set_manager->bake();

  m_have_active_scene = true;
}

const renderer_capabilities &renderer::get_capabilities() const /*override*/
{
  static renderer_capabilities s_empty;
  return layer_abstraction_factory::instance()
      .get_vulkan_context()
      .get_capabilities();
}

descriptor_set_manager &renderer::get_descriptor_set_manager() {
  return *m_descriptor_set_manager;
}

}  // namespace wunder::vulkan