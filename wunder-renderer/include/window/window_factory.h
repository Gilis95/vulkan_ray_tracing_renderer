#ifndef WINDOW_FACTORY_H
#define WINDOW_FACTORY_H

#include "core/wunder_memory.h"
#include "window/window.h"

namespace wunder {
enum class window_type;

class window_factory {
 private:
  window_factory() = default;

 public:
  static window_factory& get_instance() { return s_instance; }

 public:
  bool create_window(window_type type);
  window& get_window();

 private:
  static window_factory s_instance;

  unique_ptr<window> m_window;
};
}  // namespace wunder

#endif /* WINDOW_FACTORY_H */
