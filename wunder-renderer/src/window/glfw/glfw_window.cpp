#include "window/glfw/glfw_window.h"

#include <GLFW/glfw3.h>

#include <vector>

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "event/event_controller.h"
#include "event/input_events.h"
#include "event/window_events.h"
#include "window/window_properties.h"

namespace wunder {

/////////////////////////////////////////////////////////////////////////////////////////
static void glfw_error_callback(int error, const char *description) {
  WUNDER_ERROR("GLFW Error ({0}): {1}", error, description);
}

/////////////////////////////////////////////////////////////////////////////////////////
glfw_window::glfw_window() = default;

/////////////////////////////////////////////////////////////////////////////////////////
glfw_window::~glfw_window() = default;

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::init(const window_properties &properties) {
  int status = glfwInit();

  AssertReturnUnless(status == GLFW_TRUE);
  glfwSetErrorCallback(glfw_error_callback);

  m_window = glfwCreateWindow(properties.m_width, properties.m_height,
                              properties.m_title.c_str(), NULL, NULL);

  // create context for current window
  glfwMakeContextCurrent(m_window);
  glfwSetWindowCloseCallback(m_window, &on_close);

  init_input_event_listeners();

  // VSync
  glfwSwapInterval(0);
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::update(time_unit dt) {
  // poll for process events
  glfwPollEvents();

  // swap front and back buffer
  glfwSwapBuffers(m_window);
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::shutdown() { glfwTerminate(); }

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::on_close(GLFWwindow *window) {
  event_controller::on_event<wunder::event::window_close_event>(
      wunder::event::window_close_event{});
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::fill_vulkan_extensions(
    wunder::vulkan::vulkan_extensions &out_extensions) const {
  std::uint32_t count = 0;
  auto extensions = glfwGetRequiredInstanceExtensions(&count);
  ReturnUnless(count > 0);

  size_t old_size = out_extensions.m_extensions.size();
  out_extensions.m_extensions.resize(old_size + count);
  memcpy(out_extensions.m_extensions.data(), extensions,
         count * sizeof(extensions));
}
void glfw_window::init_input_event_listeners() {
  glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int scanCode,
                                  int action, int mods) {
    switch (action) {
      case GLFW_PRESS:
      case GLFW_REPEAT: {
        wunder::event::keyboard::pressed press_event(
            static_cast<wunder::keyboard::key_code>(key));

        event_controller::on_event(press_event);
        break;
      }
      case GLFW_RELEASE: {
        wunder::event::keyboard::pressed release_event(
            static_cast<wunder::keyboard::key_code>(key));
        event_controller::on_event(release_event);

        break;
      }
    }
  });

  glfwSetCharCallback(m_window, [](GLFWwindow *window, unsigned int key_code) {
    wunder::event::keyboard::symbol_pressed press_event(
        static_cast<wunder::keyboard::key_code>(key_code));

    event_controller::on_event(press_event);
  });

  glfwSetScrollCallback(
      m_window, [](GLFWwindow *window, double x_offset, double y_offset) {
        wunder::event::mouse::scroll mouse_scrolled_event(
            glm::vec2(x_offset, y_offset));
        event_controller::on_event(mouse_scrolled_event);
      });

  glfwSetCursorPosCallback(
      m_window, [](GLFWwindow *window, double x_pos, double y_pos) {
        wunder::event::mouse::move mouse_moved_event(glm::vec2(x_pos, y_pos));
        event_controller::on_event(mouse_moved_event);
      });

  glfwSetMouseButtonCallback(
      m_window, [](GLFWwindow *window, int button, int action, int mods) {
        switch (action) {
          case GLFW_PRESS: {
            wunder::event::mouse::pressed mouse_pressed_event(
                static_cast<wunder::mouse::key_code>(button));
            event_controller::on_event(mouse_pressed_event);

            break;
          }
          case GLFW_RELEASE: {
            wunder::event::mouse::released mouse_released_event(
                static_cast<wunder::mouse::key_code>(button));
            event_controller::on_event(mouse_released_event);

            break;
          }
        }
      });
}
}  // namespace wunder
