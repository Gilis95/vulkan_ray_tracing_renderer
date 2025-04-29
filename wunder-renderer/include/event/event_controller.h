//
// Created by christian on 7/3/24.
//

#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_CONTROLLER_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_CONTROLLER_H_
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
#include <functional>
#include <vector>

#include "event/event_handler.h"

namespace wunder {

namespace event_handlers {
/////////////////////////////////////////////////////////////////////////////////////////
template <typename event_type>
struct event_handler_container {
  static std::vector<event_handler<event_type>*> container;
};

template <typename event_type>
std::vector<event_handler<event_type>*>
    event_handler_container<event_type>::container;
}  // namespace event_handlers

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class event_controller {
 public:
  event_controller() = default;

  /////////////////////////////////////////////////////////////////////////////////////////
  template <typename event_type>
  static void register_event_handler(event_handler<event_type>* listener) {
    event_handlers::event_handler_container<event_type>::container.emplace_back(
        listener);
  }

  template <typename event_type>
  static void unregister_event_handler(
      const event_handler<event_type>* listener) {
    auto from_pos = std::remove_if(
        event_handlers::event_handler_container<event_type>::container.begin(),
        event_handlers::event_handler_container<event_type>::container.end(),
        [listener](const event_handler<event_type>* element) {
          return element == listener;
        });

    event_handlers::event_handler_container<event_type>::container.erase(
        from_pos,
        event_handlers::event_handler_container<event_type>::container.end());
  }

  /////////////////////////////////////////////////////////////////////////////////////////
  template <typename event_type>
  static void on_event(const event_type& event) {
    std::for_each(
        event_handlers::event_handler_container<event_type>::container.begin(),
        event_handlers::event_handler_container<event_type>::container.end(),
        [&](event_handler<event_type>* listener) -> void {
          listener->on_event(event);
        });
  }
};
}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_CONTROLLER_H_
