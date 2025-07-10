#include "window_helper.h"

#include <GLFW/glfw3.h>

#include "core/wunder_macros.h"

namespace wunder {
float get_dpi_scale() {
  // Cached DPI scale, so that this doesn't change after the first time code
  // calls getDPIScale. A negative value indicates that the value hasn't been
  // computed yet.
  static float cached_dpi_scale = -1.0f;

  if (cached_dpi_scale < 0.0f) {
    // Compute the product of the monitor DPI scale and any DPI scale
    // set in the NVPRO_DPI_SCALE variable.
    cached_dpi_scale = 1.0f;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    AssertReturnUnless(monitor, 1.0f);

    float y_scale;
    glfwGetMonitorContentScale(monitor, &cached_dpi_scale, &y_scale);

    cached_dpi_scale = (cached_dpi_scale > 0.0f ? cached_dpi_scale : 1.0f);
  }

  return cached_dpi_scale;
}
}