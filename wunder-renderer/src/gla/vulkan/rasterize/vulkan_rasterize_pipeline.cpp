#include "gla/vulkan/rasterize/vulkan_rasterize_pipeline.h"

#include "gla/vulkan/rasterize/vulkan_render_pass.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_renderer_context.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
rasterize_pipeline::rasterize_pipeline() noexcept
    : base_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS),
      m_pipeline_create_info{},
      m_pipeline_cache(VK_NULL_HANDLE),
      m_pipeline_state{} {}

std::unique_ptr<rasterize_pipeline> rasterize_pipeline::create(
    const descriptor_set_manager& descriptor_set_manager,
    const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
        shaders,
    render_pass& render_pass) {
  unique_ptr<rasterize_pipeline> pipeline;
  pipeline.reset(new rasterize_pipeline());

  pipeline->initialize_pipeline_layout(descriptor_set_manager);
  pipeline->initialize_pipeline(shaders, render_pass);

  return pipeline;
}

[[nodiscard]] VkPushConstantRange rasterize_pipeline::get_push_constant_range()
    const {
  return VkPushConstantRange{VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                             sizeof(Tonemapper)};
}

void rasterize_pipeline::initialize_pipeline(
    const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
        shaders_of_types,
    render_pass& render_pass) {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();

  device& device = vulkan_context.mutable_device();

  create_shader_stage_create_info(shaders_of_types);

  m_pipeline_create_info.sType =
      VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  m_pipeline_state.add_state_to(m_pipeline_create_info);

  m_pipeline_create_info.layout = m_vulkan_pipeline_layout;
  m_pipeline_create_info.renderPass = render_pass.get_vulkan_render_pass();
  m_pipeline_create_info.stageCount = static_cast<uint32_t>(
      m_shader_stage_create_infos.size());  // Stages are shaders
  m_pipeline_create_info.pStages = m_shader_stage_create_infos.data();

  vkCreateGraphicsPipelines(device.get_vulkan_logical_device(),
                            m_pipeline_cache, 1, &m_pipeline_create_info,
                            nullptr, &m_vulkan_pipeline);
}

}  // namespace wunder::vulkan