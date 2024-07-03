#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/wunder_memory.h"
#include "gla/graphic_layer_abstraction_factory.h"
#include "window/window.h"
#include "event/event_handler.h"
#include "event/window_events.h"

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
namespace wunder {
struct application_properties {
  std::string m_debug_name;
  std::string m_debug_version;
  window_properties m_window_properties;
  renderer_properties m_renderer_properties;
};

class application : private event_handler<window_close_event>{
 public:
  explicit application(application_properties properties);
  ~application() override;

  /**
   * This is second function called after class creation.
   * It's used for initializing debugger, logger and window.
   */
  void init();

  virtual void init_internal() = 0;

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
  void on_event(const window_close_event&) override;

 private:
  bool m_is_running;
  application_properties m_properties;
};

extern application *create_application();

}  // namespace wunder
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#endif
