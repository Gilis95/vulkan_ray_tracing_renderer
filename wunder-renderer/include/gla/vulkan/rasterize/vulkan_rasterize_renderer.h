#ifndef WUNDER_VULKAN_RASTERIZE_RENDERER_H
#define WUNDER_VULKAN_RASTERIZE_RENDERER_H

#include <glad/vulkan.h>

#include "core/wunder_memory.h"
#include "gla/renderer_api.h"
#include "gla/vulkan/vulkan_base_renderer.h"
#include "gla/vulkan/vulkan_texture_fwd.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
class swap_chain;
class descriptor_set_manager;
class rasterize_pipeline;
}  // namespace wunder::vulkan

namespace wunder::vulkan {

class rasterize_renderer : public base_renderer {
 public:
  explicit rasterize_renderer(const renderer_properties& m_renderer_properties);
  ~rasterize_renderer();

 public:
  void initialize();

  storage_texture& get_output_image();

 public:
  void begin_frame();

  void draw_frame();

  void end_frame();

 private:
  void create_descriptor_manager(const shader& shader);

  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
  get_shaders_for_compilation() override;

 private:
  const renderer_properties& m_renderer_properties;
  VkRect2D m_render_region;

  Tonemapper m_tonemapper;

  unique_ptr<rasterize_pipeline> m_pipeline;

  unique_ptr<storage_texture> m_output_image;
  unique_ptr<sampled_texture> m_input_image;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_RASTERIZE_RENDERER_H
