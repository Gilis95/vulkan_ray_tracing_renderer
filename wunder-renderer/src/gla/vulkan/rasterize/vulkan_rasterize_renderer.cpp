#include "gla/vulkan/rasterize/vulkan_rasterize_renderer.h"

#include "core/wunder_macros.h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/rasterize/vulkan_rasterize_pipeline.h"
#include "gla/vulkan/rasterize/vulkan_render_pass.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_texture.h"
#include "gla/vulkan/vulkan_shader.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
rasterize_renderer::rasterize_renderer(
    const renderer_properties& renderer_properties)
    : m_renderer_properties(renderer_properties),
      m_swap_chain(make_unique<swap_chain>(m_renderer_properties.m_width,
                                           m_renderer_properties.m_height)),
      m_pipeline(make_unique<rasterize_pipeline>()) {}

rasterize_renderer::~rasterize_renderer() = default;

void rasterize_renderer::initialize() {
  m_swap_chain->initialize();

  m_output_image.reset(new storage_texture(
      {.m_enabled = true, .m_descriptor_name = "rtxGeneratedImage"},
      VK_FORMAT_R32G32B32A32_SFLOAT, m_renderer_properties.m_width,
      m_renderer_properties.m_height));
  m_input_image.reset(new sampled_texture(*m_output_image));

  initialize_shaders();
  m_pipeline->initialize_pipeline(m_shaders,
                                  m_swap_chain->mutable_render_pass());

  m_input_image->add_descriptor_to(*this);
}

storage_texture& rasterize_renderer::get_output_image(){
  return *m_output_image;
}

void rasterize_renderer::create_descriptor_manager(const shader& shader) {
  m_descriptor_set_manager.reset(new descriptor_set_manager());
  m_descriptor_set_manager->initialize(shader);
}

vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
rasterize_renderer::get_shaders_for_compilation() {
  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
      shaders_to_compile;
  shaders_to_compile[VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT] =
      std::vector<shader_to_compile>(
          std::initializer_list<shader_to_compile>{shader_to_compile{
              .m_shader_path =
                  "wunder-renderer/resources/shaders/rasterize/post.frag",
              .m_on_successful_compile =
                  std::bind(&rasterize_renderer::create_descriptor_manager,
                            this, std::placeholders::_1),
              .m_optional = false}});

  shaders_to_compile[VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT] =
      std::vector<shader_to_compile>(std::initializer_list<
                                     shader_to_compile>{shader_to_compile{
          .m_shader_path =
              "wunder-renderer/resources/shaders/rasterize/passthrough.vert",
          .m_on_successful_compile =
              std::bind(&rasterize_renderer::create_descriptor_manager, this,
                        std::placeholders::_1),
          .m_optional = false}});

  return shaders_to_compile;
}

void rasterize_renderer::begin_frame() {
  AssertReturnUnless(m_swap_chain->acquire().has_value(), );
}

void rasterize_renderer::end_frame() {
  m_swap_chain->begin_render_pass();

  auto size =
      glm::vec2(m_renderer_properties.m_width, m_renderer_properties.m_height);
  auto area =
      glm::vec2(m_render_region.extent.width, m_render_region.extent.height);

  VkViewport viewport{static_cast<float>(m_render_region.offset.x),
                      static_cast<float>(m_render_region.offset.y),
                      static_cast<float>(m_renderer_properties.m_width),
                      static_cast<float>(m_renderer_properties.m_height),
                      0.0f,
                      1.0f};
  VkRect2D scissor{
      m_render_region.offset,
      {m_render_region.extent.width, m_render_region.extent.height}};

  command_pool& command_pool = layer_abstraction_factory::instance()
                                   .get_vulkan_context()
                                   .get_device()
                                   .get_command_pool();
  auto graphic_command_buffer =
      command_pool.get_current_graphics_command_buffer();

  vkCmdSetViewport(graphic_command_buffer, 0, 1, &viewport);
  vkCmdSetScissor(graphic_command_buffer, 0, 1, &scissor);

  vkCmdPushConstants(
      graphic_command_buffer, m_pipeline->get_vulkan_pipeline_layout(),
      VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(Tonemapper), &m_tonemapper);
  m_pipeline->bind();
  m_descriptor_set_manager->bind(*m_pipeline);

  vkCmdDraw(graphic_command_buffer, 3, 1, 0, 0);

  m_swap_chain->end_render_pass();
}

}  // namespace wunder::vulkan