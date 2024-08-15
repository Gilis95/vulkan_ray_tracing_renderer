#include "window/window_factory.h"

#include "core/wunder_macros.h"
#include "window/window.h"
#include "window/window_properties.h"
#include "window/glfw/glfw_window.h"

namespace wunder {
window_factory window_factory::s_instance;

bool window_factory::create_window(const window_properties & window_properties) {
  switch (window_properties.m_type) {
    case window_type::glfw: {
      m_window = make_unique<wunder::glfw_window>();
    }
      break;
    default:
      AssertReturnIf("Not handled window type.", false)
  }

  m_window->init(window_properties);
  return true;
}

window& window_factory::get_window() { return *m_window; }
}  // namespace wunder
