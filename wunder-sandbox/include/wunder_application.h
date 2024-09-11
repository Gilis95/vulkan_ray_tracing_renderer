#ifndef WANDER_APPLICATION_H
#define WANDER_APPLICATION_H

#include <application.h>
#include <event/event_handler.h>
//#include <event/scene_events.h>

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
namespace wunder {
struct scene_loaded;
class wunder_application : public application,
                           public event_handler<scene_loaded> {
 public:
  explicit wunder_application(application_properties&& properties);
~wunder_application() override;
 public:
  void initialize_internal() override;
 public:
  void on_event(const scene_loaded&) override;
};

/////////////////////////////////////////////////////////////////////////////////////////
application* create_application();
}  // namespace wunder
#endif
