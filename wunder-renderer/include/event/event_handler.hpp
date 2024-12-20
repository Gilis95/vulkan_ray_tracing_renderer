
#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_HANDLER_HPP_
#define WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_HANDLER_HPP_
#include "event/event_handler.h"
#include "event/event_controller.h"

namespace wunder {
template <typename event_type>
event_handler<event_type>::event_handler() {
  event_controller::register_event_handler<event_type>(this);
}

template <typename event_type>
event_handler<event_type>::~event_handler() {
  event_controller::unregister_event_handler<event_type>(this);
}
}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_HANDLER_HPP_