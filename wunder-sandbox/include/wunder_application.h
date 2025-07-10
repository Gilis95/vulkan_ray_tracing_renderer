#ifndef WANDER_APPLICATION_H
#define WANDER_APPLICATION_H

#include <application.h>
#include <event/event_handler.h>

#include "imgui/wunder_imgui.h"
// #include <event/scene_events.h>

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

namespace wunder::event {
struct scene_loaded;
}

namespace wunder {
class wunder_application : public application,
                           public event_handler<wunder::event::scene_loaded> {
 public:
  explicit wunder_application(application_properties&& properties);
  ~wunder_application() override;

 private:
  void initialize_imgui();
  void initialize_internal() override;
  void shutdown_internal() override;

  void update_internal(const time_unit& time_unit) override;
 public:
  void on_event(const wunder::event::scene_loaded&) override;

 private:
  wunder_imgui m_imgui;
};
/////////////////////////////////////////////////////////////////////////////////////////
application* create_application();
}  // namespace wunder
#endif
