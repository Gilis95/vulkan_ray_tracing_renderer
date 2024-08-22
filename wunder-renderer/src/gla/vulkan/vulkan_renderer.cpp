#include "gla/vulkan/vulkan_renderer.h"

#include <glad/vulkan.h>

#include <optional>

#include "gla/renderer_capabilities .h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_pipeline.h"
#include "gla/vulkan/vulkan_shader.h"
#include "gla/vulkan/vulkan_shader_binding_table.h"

namespace wunder {

vulkan_renderer::~vulkan_renderer() = default;

void vulkan_renderer::init_internal(const renderer_properties &properties) {
  m_pipeline = std::make_unique<vulkan_pipeline>();
  m_shader_binding_table = std::make_unique<vulkan_shader_binding_table>();

  for (auto &[shader_type, shaders_compile_data] :
       get_shaders_for_compilation()) {
    auto &shaders_of_type = m_shaders[shader_type];

    for (auto &shader_compile_data : shaders_compile_data) {
      auto maybe_shader =
          vulkan_shader::create(shader_compile_data.m_shader_path, shader_type);
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
vulkan_renderer::get_shaders_for_compilation() {
  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
      shaders_to_compile;
  shaders_to_compile[VkShaderStageFlagBits::VK_SHADER_STAGE_RAYGEN_BIT_KHR] =
      std::vector<shader_to_compile>(
          std::initializer_list<shader_to_compile>{shader_to_compile{
              .m_shader_path =
                  "wunder-renderer/resources/shaders/pathtrace.rgen",
              .m_on_successful_compile =
                  std::bind(&vulkan_renderer::create_descriptor_manager, this,
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

void vulkan_renderer::create_descriptor_manager(const vulkan_shader &shader) {
  m_descriptor_set_manager = std::make_unique<vulkan_descriptor_set_manager>();
  m_descriptor_set_manager->initialize(shader);

  m_pipeline->create_pipeline_layout(shader);
}

void vulkan_renderer::update(int dt) /*override*/
{
  auto graphic_command_buffer = vulkan_layer_abstraction_factory::instance()
                                    .get_vulkan_context()
                                    .get_device()
                                    .get_graphics_queue();
  //  vkCmdBindPipeline(graphic_command_buffer,
  //  VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_rtPipeline);
  //  vkCmdBindDescriptorSets(
  //      graphic_command_buffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
  //      m_rtPipelineLayout, 0,
  //                          static_cast<uint32_t>(descSets.size()),
  //                          descSets.data(), 0, nullptr);
  //  vkCmdPushConstants(graphic_command_buffer, m_rtPipelineLayout,
  //                     VK_SHADER_STAGE_RAYGEN_BIT_KHR |
  //                     VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR |
  //                     VK_SHADER_STAGE_MISS_BIT_KHR, 0, sizeof(RtxState),
  //                     &m_state);
  //
  //
  //  auto& regions = m_sbtWrapper.getRegions();
  //  vkCmdTraceRaysKHR(graphic_command_buffer, &regions[0], &regions[1],
  //  &regions[2], &regions[3], size.width, size.height, 1);
}

const renderer_capabilities &vulkan_renderer::get_capabilities()
    const /*override*/
{
  static renderer_capabilities s_empty;
  return vulkan_layer_abstraction_factory::instance()
      .get_vulkan_context()
      .get_capabilities();
}

}  // namespace wunder