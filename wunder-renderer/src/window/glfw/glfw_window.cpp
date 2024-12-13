#include "window/glfw/glfw_window.h"

#include <GLFW/glfw3.h>

#include <vector>

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "event/event_controller.h"
#include "event/window_events.h"
#include "window/window_properties.h"

namespace wunder {

/////////////////////////////////////////////////////////////////////////////////////////
static void glfw_error_callback(int error, const char* description) {
  WUNDER_ERROR("GLFW Error ({0}): {1}", error, description);
}

/////////////////////////////////////////////////////////////////////////////////////////
glfw_window::glfw_window() = default;

/////////////////////////////////////////////////////////////////////////////////////////
glfw_window::~glfw_window() = default;

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::init(const window_properties& properties) {
  int status = glfwInit();

  AssertReturnUnless(status == GLFW_TRUE);
  glfwSetErrorCallback(glfw_error_callback);

  m_window = glfwCreateWindow(properties.m_width, properties.m_height,
                              properties.m_title.c_str(), NULL, NULL);

  // create context for current window
  glfwMakeContextCurrent(m_window);
  glfwSetWindowCloseCallback(m_window, &on_close);

  // VSync
  glfwSwapInterval(0);
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::update(int dt) {
  // poll for process events
  glfwPollEvents();

  // swap front and back buffer
  glfwSwapBuffers(m_window);
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::shutdown() { glfwTerminate(); }

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::on_close(GLFWwindow* window) {
  event_controller::on_event<window_close_event>(window_close_event{});
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::fill_vulkan_extensions(
    wunder::vulkan::vulkan_extensions& out_extensions) const {
  std::uint32_t count = 0;
  auto extensions = glfwGetRequiredInstanceExtensions(&count);
  ReturnUnless(count > 0);

  size_t old_size = out_extensions.m_extensions.size();
  out_extensions.m_extensions.resize(old_size + count);
  memcpy(out_extensions.m_extensions.data(), extensions,
         count * sizeof(extensions));
}
}  // namespace wunder
