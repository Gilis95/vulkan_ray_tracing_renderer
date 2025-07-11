#ifndef WUNDER_IMGUI_H
#define WUNDER_IMGUI_H
#include <glad/vulkan.h>

#include "core/time_unit.h"
#include "event/event_handler.h"
#include "event/vulkan_events.h"
#include "imgui/imgui_right_side_panel.h"

namespace wunder {
namespace vulkan {
class render_pass;
}

namespace event {
class scene_activated;
namespace vulkan {
class renderer_shutdown;
}
}  // namespace event

}  // namespace wunder
namespace wunder {

class wunder_imgui : private event_handler<wunder::event::scene_activated>,
                     private event_handler<event::vulkan::renderer_shutdown> {
 public:
  wunder_imgui();
  ~wunder_imgui() override;

 public:
  void init();
  void shutdown();

 public:
  void update(wunder::time_unit dt);

 private:
  void on_event(const wunder::event::scene_activated& event) override;
  void on_event(const wunder::event::vulkan::renderer_shutdown& event) override;

 private:
  VkAttachmentLoadOp m_load_op;
  right_side_panel m_right_side_panel;
  VkDescriptorPool m_imgui_desc_pool;
};
}  // namespace wunder
#endif  // WUNDER_IMGUI_H
