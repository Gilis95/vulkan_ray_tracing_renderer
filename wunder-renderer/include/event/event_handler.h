#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_HANDLER_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_HANDLER_H_

namespace wunder {
template <typename event_type>
class event_handler {
 public:
  event_handler();
  virtual ~event_handler();

  virtual void on_event(const event_type&) = 0;
};

}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_EVENT_EVENT_HANDLER_H_
