#ifndef VULKAN_RENDERER_CONTEXT_H
#define VULKAN_RENDERER_CONTEXT_H

#include "core/non_copyable.h"
#include "core/wunder_memory.h"
#include "event/event_handler.h"

// forward declarations
namespace wunder {
class time_unit;
struct renderer_properties;
struct renderer_capabilities;
}  // namespace wunder

namespace wunder::event {
struct scene_activated;
}  // namespace wunder::event

namespace wunder::vulkan {
class swap_chain;
class rasterize_renderer;
class rtx_renderer;

class renderer_context : public non_copyable,
                         public event_handler<wunder::event::scene_activated> {
private:
  friend class layer_abstraction_factory;
  public:
  renderer_context(const renderer_properties& properties);
  ~renderer_context() override;

 private:
  void init();
  void shutdown();

 public:
  const renderer_properties& get_renderer_properties() const;
  swap_chain& mutable_swap_chain();
  rasterize_renderer& mutable_rasterize_renderer();
  rtx_renderer& mutable_rtx_renderer();

 public:
  void update(time_unit dt);

 private:
  void on_event(const wunder::event::scene_activated&) override;

 private:
  void log_current_sate_frame();
  void log_loaded_scene_size();

 private:
  bool m_have_active_scene;

  const renderer_properties& m_renderer_properties;
  unique_ptr<swap_chain> m_swap_chain;
  unique_ptr<rasterize_renderer> m_rasterize_renderer;
  unique_ptr<rtx_renderer> m_rtx_renderer;
};
}  // namespace wunder::vulkan
#endif  // VULKAN_RENDERER_CONTEXT_H
