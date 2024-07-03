#include "window/window_factory.h"

#include "window/window.h"
#include "window/glfw/glfw_window.h"

namespace wunder {
  window_factory  window_factory::s_instance;

  bool window_factory::create_window(window_type type)  {
    switch(type) {
      case window_type::glfw:
        m_window = make_unique<wunder::glfw_window>();
      default:
        return false;
    }

    return true;
  }

    window& window_factory::get_window()
    {
        return *m_window;
    }
}
