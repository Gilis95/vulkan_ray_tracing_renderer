#include "gla/vulkan/rasterize/vulkan_rasterize_pipeline_state.h"

namespace {
template <class T, class U>
void set_value(T& target, const U& val) {
  target = (T)(val);
}
}  // namespace

namespace wunder::vulkan {
vulkan_rasterize_pipeline_state::vulkan_rasterize_pipeline_state()
    : m_input_assembly_state{},
      m_rasterization_state{},
      m_multisample_state{},
      m_depth_stencil_state{},
      m_viewport_state{},
      m_dynamic_state{},
      m_color_blend_state{},
      m_vertex_input_state{},
      m_dynamic_state_enables{VK_DYNAMIC_STATE_VIEWPORT,
                              VK_DYNAMIC_STATE_SCISSOR},
      m_blend_attachment_states{make_pipeline_color_blend_attachment_state()}
{
  m_input_assembly_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  m_rasterization_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  m_multisample_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  m_depth_stencil_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  m_viewport_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  m_dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  m_color_blend_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  m_vertex_input_state.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  m_rasterization_state.flags = {};
  m_rasterization_state.depthClampEnable = {};
  m_rasterization_state.rasterizerDiscardEnable = {};
  set_value(m_rasterization_state.polygonMode, VK_POLYGON_MODE_FILL);
  set_value(m_rasterization_state.cullMode, VK_CULL_MODE_NONE);
  set_value(m_rasterization_state.frontFace, VK_FRONT_FACE_COUNTER_CLOCKWISE);

  m_rasterization_state.depthBiasEnable = {};
  m_rasterization_state.depthBiasConstantFactor = {};
  m_rasterization_state.depthBiasClamp = {};
  m_rasterization_state.depthBiasSlopeFactor = {};
  m_rasterization_state.lineWidth = 1.f;

  m_input_assembly_state.flags = {};
  set_value(m_input_assembly_state.topology,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
  m_input_assembly_state.primitiveRestartEnable = {};

  m_color_blend_state.flags = {};
  m_color_blend_state.logicOpEnable = {};
  set_value(m_color_blend_state.logicOp, VK_LOGIC_OP_CLEAR);
  m_color_blend_state.attachmentCount = m_blend_attachment_states.size();
  m_color_blend_state.pAttachments = m_blend_attachment_states.data();
  for (int i = 0; i < 4; i++) {
    m_color_blend_state.blendConstants[i] = 0.f;
  }

  m_dynamic_state.flags = {};
  m_dynamic_state.dynamicStateCount = m_dynamic_state_enables.size();
  m_dynamic_state.pDynamicStates = m_dynamic_state_enables.data();

  m_vertex_input_state.flags = {};
  m_vertex_input_state.vertexBindingDescriptionCount = {};
  m_vertex_input_state.pVertexBindingDescriptions = {};
  m_vertex_input_state.vertexAttributeDescriptionCount = {};
  m_vertex_input_state.pVertexAttributeDescriptions = {};

  m_viewport_state.flags = {};
  m_viewport_state.viewportCount = 1;
  m_viewport_state.pViewports = nullptr;
  m_viewport_state.scissorCount = 1;
  m_viewport_state.pScissors = nullptr;

  m_depth_stencil_state.flags = {};
  m_depth_stencil_state.depthTestEnable = VK_TRUE;
  m_depth_stencil_state.depthWriteEnable = VK_TRUE;
  set_value(m_depth_stencil_state.depthCompareOp, VK_COMPARE_OP_LESS_OR_EQUAL);
  m_depth_stencil_state.depthBoundsTestEnable = {};
  m_depth_stencil_state.stencilTestEnable = {};
  set_value(m_depth_stencil_state.front, VkStencilOpState());
  set_value(m_depth_stencil_state.back, VkStencilOpState());
  m_depth_stencil_state.minDepthBounds = {};
  m_depth_stencil_state.maxDepthBounds = {};

  set_value(m_multisample_state.rasterizationSamples, VK_SAMPLE_COUNT_1_BIT);
}

void vulkan_rasterize_pipeline_state::add_state_to(
    VkGraphicsPipelineCreateInfo& pipeline_create_info) const {
  pipeline_create_info.pRasterizationState = &m_rasterization_state;
  pipeline_create_info.pInputAssemblyState = &m_input_assembly_state;
  pipeline_create_info.pColorBlendState = &m_color_blend_state;
  pipeline_create_info.pMultisampleState = &m_multisample_state;
  pipeline_create_info.pViewportState = &m_viewport_state;
  pipeline_create_info.pDepthStencilState = &m_depth_stencil_state;
  pipeline_create_info.pDynamicState = &m_dynamic_state;
  pipeline_create_info.pVertexInputState = &m_vertex_input_state;
}

}  // namespace wunder::vulkan
