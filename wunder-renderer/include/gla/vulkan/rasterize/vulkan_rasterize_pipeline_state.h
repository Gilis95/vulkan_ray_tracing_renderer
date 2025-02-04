#ifndef WUNDER_VULKAN_RASTERIZE_PIPELINE_STATE_H
#define WUNDER_VULKAN_RASTERIZE_PIPELINE_STATE_H

#include <glad/vulkan.h>

#include <vector>

namespace wunder::vulkan {

class rasterize_pipeline;

class vulkan_rasterize_pipeline_state {
 private:
  friend rasterize_pipeline;

 public:
  vulkan_rasterize_pipeline_state();

 protected:
  void add_state_to(VkGraphicsPipelineCreateInfo& pipeline_create_info);

 private:
  static inline VkPipelineColorBlendAttachmentState
  make_pipeline_color_blend_attachment_state(
      VkColorComponentFlags colorWriteMask_ = VK_COLOR_COMPONENT_R_BIT |
                                              VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT |
                                              VK_COLOR_COMPONENT_A_BIT,
      VkBool32 blend_enable = 0,
      VkBlendFactor src_color_blend_factor = VK_BLEND_FACTOR_ZERO,
      VkBlendFactor dst_color_blend_factor = VK_BLEND_FACTOR_ZERO,
      VkBlendOp color_blend_op = VK_BLEND_OP_ADD,
      VkBlendFactor src_alpha_blend_factor = VK_BLEND_FACTOR_ZERO,
      VkBlendFactor dst_alpha_blend_factor = VK_BLEND_FACTOR_ZERO,
      VkBlendOp alpha_blend_op = VK_BLEND_OP_ADD) {
    VkPipelineColorBlendAttachmentState res;

    res.blendEnable = blend_enable;
    res.srcColorBlendFactor = src_color_blend_factor;
    res.dstColorBlendFactor = dst_color_blend_factor;
    res.colorBlendOp = color_blend_op;
    res.srcAlphaBlendFactor = src_alpha_blend_factor;
    res.dstAlphaBlendFactor = dst_alpha_blend_factor;
    res.alphaBlendOp = alpha_blend_op;
    res.colorWriteMask = colorWriteMask_;
    return res;
  }

 private:
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

  std::vector<VkDynamicState> m_dynamic_state_enables = {
      VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  std::vector<VkPipelineColorBlendAttachmentState> m_blend_attachment_states{
      make_pipeline_color_blend_attachment_state()};
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_RASTERIZE_PIPELINE_STATE_H