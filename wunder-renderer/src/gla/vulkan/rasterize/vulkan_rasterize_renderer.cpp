#include "gla/vulkan/rasterize/vulkan_rasterize_renderer.h"

#include "core/time_unit.h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/rasterize/vulkan_rasterize_pipeline.h"
#include "gla/vulkan/rasterize/vulkan_render_pass.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_renderer_context.h"
#include "gla/vulkan/vulkan_shader.h"
#include "gla/vulkan/vulkan_texture.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
rasterize_renderer::rasterize_renderer(
    const renderer_properties &renderer_properties)
    : m_render_region{.offset = {.x = 0, .y = 0},
                      .extent = {.width = renderer_properties.m_width,
                                 .height = renderer_properties.m_height}},
      m_tonemapper{.brightness = 1.0f,
                   .contrast = 1.0f,
                   .saturation = 1.0f,
                   .vignette = 0.0f,
                   .avgLum = 1.0f,
                   .zoom = 1.0f,
                   .renderingRatio = {1.34, 1.03},
                   .autoExposure = 0,
                   .Ywhite = 0.5f,
                   .key = 0.5f,
                   .dither = 1} {
  m_output_image.reset(new storage_texture(
      {.m_enabled = true, .m_descriptor_name = "rtxGeneratedImage"},
      VK_FORMAT_R32G32B32A32_SFLOAT, renderer_properties.m_width,
      renderer_properties.m_height));
  m_input_image.reset(new sampled_texture(*m_output_image));
}

rasterize_renderer::~rasterize_renderer() = default;

void rasterize_renderer::update(time_unit /*dt*/) /*override*/ {
  begin_frame();
  draw_frame();
  end_frame();
}

void rasterize_renderer::shutdown_internal() /*override*/ {
  m_output_image.reset();
  m_input_image.reset();
  m_pipeline.reset();
}

void rasterize_renderer::init_internal(scene_id /*id*/) /*override*/ {
  m_input_image->add_descriptor_to(*m_descriptor_set_manager);
  m_descriptor_set_manager->build();

  m_pipeline = std::move(rasterize_pipeline::create(*m_descriptor_set_manager,
                                                    m_shaders));
}

void rasterize_renderer::create_descriptor_manager(const shader &shader) {
  m_descriptor_set_manager.reset(
      new descriptor_set_manager(shader.get_shader_reflection_data()));
}

vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
rasterize_renderer::get_shaders_for_compilation() {
  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
      shaders_to_compile;
  shaders_to_compile[VK_SHADER_STAGE_FRAGMENT_BIT] =
      std::vector(std::initializer_list{shader_to_compile{
          .m_shader_path =
              "wunder-renderer/resources/shaders/rasterize/post.frag",
          .m_on_successful_compile =
              std::bind(&rasterize_renderer::create_descriptor_manager, this,
                        std::placeholders::_1),
          .m_optional = false}});

  shaders_to_compile[VK_SHADER_STAGE_VERTEX_BIT] =
      std::vector(std::initializer_list{shader_to_compile{
          .m_shader_path =
              "wunder-renderer/resources/shaders/rasterize/passthrough.vert",
          .m_on_successful_compile = nullptr,
          .m_optional = false}});

  return shaders_to_compile;
}

void rasterize_renderer::begin_frame() {
  auto &render_context =
      layer_abstraction_factory::instance().get_render_context();
  auto &renderer_properties = render_context.get_renderer_properties();
  auto &render_pass = render_context.mutable_render_pass();
  auto &swap_chain = render_context.mutable_swap_chain();
  auto graphic_command_buffer = swap_chain.get_current_command_buffer();

  m_input_image->generate_mip_levels(graphic_command_buffer);
  render_pass.push_load_operation(VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR);
  render_pass.begin();

  VkViewport viewport{static_cast<float>(m_render_region.offset.x),
                      static_cast<float>(m_render_region.offset.y),
                      static_cast<float>(renderer_properties.m_width),
                      static_cast<float>(renderer_properties.m_height),
                      0.0f,
                      1.0f};
  VkRect2D scissor{
      m_render_region.offset,
      {m_render_region.extent.width, m_render_region.extent.height}};

  vkCmdSetViewport(graphic_command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(graphic_command_buffer, 0, 1, &scissor);
}

void rasterize_renderer::draw_frame() {
  auto &render_context =
      layer_abstraction_factory::instance().get_render_context();

  auto &swap_chain = render_context.mutable_swap_chain();
  auto graphic_command_buffer = swap_chain.get_current_command_buffer();

  vkCmdPushConstants(
      graphic_command_buffer, m_pipeline->get_vulkan_pipeline_layout(),
      VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Tonemapper), &m_tonemapper);
  m_pipeline->bind();
  m_descriptor_set_manager->bind(*m_pipeline);

  vkCmdDraw(graphic_command_buffer, 3, 1, 0, 0);
}

void rasterize_renderer::end_frame() {
  auto &render_pass = layer_abstraction_factory::instance()
                          .get_render_context()
                          .mutable_render_pass();
  render_pass.end();
}

}  // namespace wunder::vulkan
