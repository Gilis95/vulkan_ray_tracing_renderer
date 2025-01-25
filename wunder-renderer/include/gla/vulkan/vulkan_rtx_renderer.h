#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <glad/vulkan.h>

#include <filesystem>
#include <string>
#include <vector>

#include "core/non_copyable.h"
#include "core/time_unit.h"
#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "event/event_handler.h"
#include "gla/renderer_api.h"
#include "gla/vulkan/vulkan_texture_fwd.h"

struct RtxState;

namespace wunder::event {
struct scene_activated;
}

namespace wunder::vulkan {

class descriptor_set_manager;
class shader;
class rtx_pipeline;
class shader_binding_table;

struct shader_to_compile {
  std::filesystem::path m_shader_path;
  std::function<void(const shader&)> m_on_successful_compile;
  bool m_optional = true;
};

class rtx_renderer : public renderer_api,
                 public event_handler<wunder::event::scene_activated>,
                 public non_copyable {
 public:
  explicit rtx_renderer(const renderer_properties&);

 public:
  ~rtx_renderer() override;

 public:
  [[nodiscard]] const renderer_capabilities& get_capabilities() const override;
  descriptor_set_manager& get_descriptor_set_manager();

  void update(time_unit dt) override;

 protected:
  void init_internal(const renderer_properties& properties) override;

  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
  get_shaders_for_compilation();

  void create_descriptor_manager(const shader& shader);

 public:
  void on_event(const wunder::event::scene_activated&) override;

 private:
  const renderer_properties& m_renderer_properties;
  bool m_have_active_scene;

  VkSurfaceKHR m_surface = VK_NULL_HANDLE;  // Vulkan window surface
  vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>> m_shaders;
  unique_ptr<descriptor_set_manager> m_descriptor_set_manager;
  unique_ptr<rtx_pipeline> m_rtx_pipeline;
  unique_ptr<shader_binding_table> m_shader_binding_table;
  unique_ptr<RtxState> m_state;

  unique_ptr<storage_texture> m_rtx_generated_image;
};
}  // namespace wunder::vulkan
#endif /* VULKAN_RENDERER_H */
