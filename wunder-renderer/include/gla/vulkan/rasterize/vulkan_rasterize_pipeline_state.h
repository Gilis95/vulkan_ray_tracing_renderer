#ifndef WUNDER_VULKAN_RASTERIZE_PIPELINE_STATE_H
#define WUNDER_VULKAN_RASTERIZE_PIPELINE_STATE_H

#include <glad/vulkan.h>
namespace wunder::vulkan {

class vulkan_rasterize_pipeline;

class vulkan_rasterize_pipeline_state {
 private:
  friend vulkan_rasterize_pipeline;
 public:
  vulkan_rasterize_pipeline_state();
 protected:
  void add_state_to(VkGraphicsPipelineCreateInfo& pipeline_create_info);
 private:
  VkSampleMask sampleMask{~0U};
  VkPipelineInputAssemblyStateCreateInfo m_input_assembly_state{
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
  VkPipelineRasterizationStateCreateInfo m_rasterization_state{
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
  VkPipelineMultisampleStateCreateInfo m_multisample_state{
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
  VkPipelineDepthStencilStateCreateInfo m_depth_stencil_state{
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
  VkPipelineViewportStateCreateInfo m_viewport_state{
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
  VkPipelineDynamicStateCreateInfo m_dynamic_state{
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
  VkPipelineColorBlendStateCreateInfo m_color_blend_state{
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
  VkPipelineVertexInputStateCreateInfo m_vertex_input_state{
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
};
}
#endif  // WUNDER_VULKAN_RASTERIZE_PIPELINE_STATE_H