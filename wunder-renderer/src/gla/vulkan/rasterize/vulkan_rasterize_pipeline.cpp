#include "gla/vulkan/rasterize/vulkan_rasterize_pipeline.h"

#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/rasterize/vulkan_render_pass.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
rasterize_pipeline::rasterize_pipeline() noexcept
    : base_pipeline(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS),
      m_pipeline_create_info(),
      m_pipeline_cache(VK_NULL_HANDLE) {}

[[nodiscard]] VkPushConstantRange
rasterize_pipeline::get_push_constant_range() const {
  return VkPushConstantRange{VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                             sizeof(Tonemapper)};
}

void rasterize_pipeline::initialize_pipeline(
    const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
        shaders_of_types,
    render_pass& renderPass) {
  device& device =
      layer_abstraction_factory::instance().get_vulkan_context().get_device();

  std::vector<VkPipelineShaderStageCreateInfo> stages =
      get_shader_stage_create_info(shaders_of_types);

  memset(&m_pipeline_create_info, 0, sizeof(VkGraphicsPipelineCreateInfo));

  m_pipeline_create_info.sType =
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  m_pipeline_state.add_state_to(m_pipeline_create_info);

  m_pipeline_create_info.layout = m_vulkan_pipeline_layout;
  m_pipeline_create_info.renderPass = renderPass.get_vulkan_render_pass();
  m_pipeline_create_info.stageCount =
      static_cast<uint32_t>(stages.size());  // Stages are shaders
  m_pipeline_create_info.pStages = stages.data();

  vkCreateGraphicsPipelines(
      device.get_vulkan_logical_device(), m_pipeline_cache, 1,
      (VkGraphicsPipelineCreateInfo*)&m_pipeline_create_info, nullptr,
      &m_vulkan_pipeline);
}

}  // namespace wunder::vulkan