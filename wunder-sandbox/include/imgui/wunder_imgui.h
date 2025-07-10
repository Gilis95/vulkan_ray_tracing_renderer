#ifndef WUNDER_IMGUI_H
#define WUNDER_IMGUI_H
#include <memory>

#include "core/time_unit.h"
#include "event/event_handler.h"
#include "imgui/imgui_right_side_panel.h"
#include <glad/vulkan.h>

namespace wunder {
namespace vulkan {
class render_pass;
}

namespace event::vulkan {
class swap_chain_destroyed;
}

}  // namespace wunder
namespace wunder {

class wunder_imgui
    : private event_handler<wunder::event::vulkan::swap_chain_destroyed> {
 public:
  wunder_imgui();
  ~wunder_imgui() override;

 public:
  void init();
  void shutdown();

 public:
  void update(wunder::time_unit dt);
private:
  void on_event(const event::vulkan::swap_chain_destroyed&) override;
 private:
  right_side_panel m_right_side_panel;
  std::unique_ptr<vulkan::render_pass> m_render_pass;
  VkDescriptorPool m_imgui_desc_pool;

};
}  // namespace wunder
#endif  // WUNDER_IMGUI_H
