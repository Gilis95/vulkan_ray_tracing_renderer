#ifndef APPLICATION_H
#define APPLICATION_H

#include "event/event_handler.h"
#include "event/window_events.h"
#include "core/wunder_memory.h"

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
namespace wunder {

class scene_manager;
class asset_manager;

struct application_properties;

class application : private event_handler<wunder::event::window_close_event>{
 public:
  explicit application(application_properties&& properties);
  ~application() override;

  /**
   * This is second function called after class creation.
   * It's used for initializing debugger, logger and window.
   */
  void initialize();

  virtual void initialize_internal() = 0;

  void close();

  /**
   * Main game loop.
   */
  void run();

  /**
   * application main loop, loops through layers queue and calls OnUpdate
   * function of each queue element. This function adds to the end of this
   * queue, provided layer.
   *
   * @param layer - layer to be pushed at the end of layers queue
   */
  //        void PushLayer(Layer *layer);

  //        void PushOverlay(Layer *layer);
 private:
  void on_event(const wunder::event::window_close_event&) override;

 private:
  bool m_is_running;
  unique_ptr<application_properties> m_properties;
};

extern application *create_application(application_properties& m_properties);

}  // namespace wunder
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#endif
