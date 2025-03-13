#include "gla/vulkan/vulkan_base_pipeline.h"

#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_shader.h"

namespace wunder::vulkan {
base_pipeline::base_pipeline(VkPipelineBindPoint bind_point)
    : m_bind_point(bind_point) {}

void base_pipeline::initialize_pipeline_layout(
    const descriptor_set_manager& descriptor_declaring_shader) {
  auto& device = layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .mutable_device();

  const auto& descriptor_sets_layout =
      descriptor_declaring_shader.get_descriptor_set_layout();

  vkDestroyPipelineLayout(device.get_vulkan_logical_device(),
                          m_vulkan_pipeline_layout, nullptr);

  // TODO:: This must be comming from shader reflect data!!!
  VkPushConstantRange push_constants = get_push_constant_range();

  VkPipelineLayoutCreateInfo pipeline_layout_create_info{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipeline_layout_create_info.pushConstantRangeCount = 1;
  pipeline_layout_create_info.pPushConstantRanges = &push_constants;
  pipeline_layout_create_info.setLayoutCount =
      static_cast<uint32_t>(descriptor_sets_layout.size());
  pipeline_layout_create_info.pSetLayouts = descriptor_sets_layout.data();
  vkCreatePipelineLayout(device.get_vulkan_logical_device(),
                         &pipeline_layout_create_info, nullptr,
                         &m_vulkan_pipeline_layout);
}

void base_pipeline::create_shader_stage_create_info(
    const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
        shaders_of_types) {

  for (auto& [shader_type, shaders] : shaders_of_types) {
    for (auto& shader : shaders) {
      m_shader_stage_create_infos .push_back(shader->get_shader_stage_info());
    }
  }
}

void base_pipeline::bind() {
  auto graphic_command_buffer = layer_abstraction_factory::instance()
                                    .get_vulkan_context()
                                    .mutable_swap_chain()
                                    .get_current_command_buffer();

  vkCmdBindPipeline(graphic_command_buffer, get_bind_point(),
                    m_vulkan_pipeline);
}

}  // namespace wunder::vulkan