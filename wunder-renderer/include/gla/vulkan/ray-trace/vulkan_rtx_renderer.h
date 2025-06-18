#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <filesystem>
#include <string>
#include <vector>

#include "core/non_copyable.h"
#include "core/time_unit.h"
#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "event/event_handler.h"
#include "gla/vulkan/vulkan_base_renderer.h"
#include "glad/vulkan.h"

struct RtxState;

namespace wunder::event {
struct scene_activated;
struct camera_moved;
}  // namespace wunder::event

namespace wunder::vulkan {

class descriptor_set_manager;
class shader;
class rtx_pipeline;
class shader_binding_table;
class rasterize_renderer;

class rtx_renderer : public base_renderer,
                     public event_handler<wunder::event::scene_activated>,
                     public event_handler<wunder::event::camera_moved>,
                     public non_copyable {
 public:
  explicit rtx_renderer(const renderer_properties&);

 public:
  ~rtx_renderer() override;

 public:
  void update(time_unit dt);

 protected:
  void shutdown_internal() override;
  void init_internal(const renderer_properties& properties) override;

  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
  get_shaders_for_compilation() override;

  void create_descriptor_manager(const shader& shader);

 private:
  void on_event(const wunder::event::scene_activated&) override;
  void on_event(const wunder::event::camera_moved&) override;

  // debug features
 private:
  void log_current_sate_frame();
  void log_loaded_scene_size();
 private:
  const renderer_properties& m_renderer_properties;
  bool m_have_active_scene;

  unique_ptr<rtx_pipeline> m_rtx_pipeline;
  unique_ptr<shader_binding_table> m_shader_binding_table;
  unique_ptr<RtxState> m_state;

  unique_ptr<rasterize_renderer> m_rasterize_renderer;
};
}  // namespace wunder::vulkan
#endif /* VULKAN_RENDERER_H */
