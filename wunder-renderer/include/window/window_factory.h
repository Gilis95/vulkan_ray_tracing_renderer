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
  static window_factory& instance();

  void shutdown();

 public:
  bool initialize(const window_properties& window_properties);
  window& get_window();

 private:
  unique_ptr<window> m_window;
};
}  // namespace wunder

#endif /* WINDOW_FACTORY_H */
