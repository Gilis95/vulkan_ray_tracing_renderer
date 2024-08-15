#include "gla/vulkan/vulkan_renderer.h"

#include <glad/vulkan.h>

#include <optional>

#include "gla/renderer_capabilities .h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_shader.h"

namespace wunder {

vulkan_renderer::~vulkan_renderer() = default;

void vulkan_renderer::init_internal(const renderer_properties &properties) {
  vulkan_shader::create("wunder-renderer/resources/shaders/pathtrace.rgen",
                        VkShaderStageFlagBits::VK_SHADER_STAGE_RAYGEN_BIT_KHR);
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