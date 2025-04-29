#include "window/glfw/glfw_window.h"

#include <GLFW/glfw3.h>

#include <vector>

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "event/event_controller.h"
#include "event/input_events.h"
#include "event/window_events.h"
#include "gla/vulkan/vulkan.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
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

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  m_window = glfwCreateWindow(properties.m_width, properties.m_height,
                              properties.m_title.c_str(), NULL, NULL);

  // create context for current window
  glfwSetWindowCloseCallback(m_window, &on_close);

  // Setup Vulkan
  if (glfwVulkanSupported() == 0) {
    WUNDER_ERROR_TAG("Window: ", "GLFW: Vulkan not supported!");
    CRASH;
  }

  init_input_event_listeners();
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::update(time_unit /*dt*/) {
  // poll for process events
  glfwPollEvents();
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::shutdown() { glfwTerminate(); }

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::on_close(GLFWwindow * /*window*/) {
  event_controller::on_event<event::window_close_event>(
      event::window_close_event{});
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::fill_vulkan_extensions(
    vulkan::vulkan_extensions &out_extensions) const {
  std::uint32_t count = 0;
  auto extensions = glfwGetRequiredInstanceExtensions(&count);
  ReturnUnless(count > 0);

  size_t old_size = out_extensions.m_extensions.size();
  out_extensions.m_extensions.resize(old_size + count);
  memcpy(out_extensions.m_extensions.data(), extensions,
         count * sizeof(extensions));
}

/////////////////////////////////////////////////////////////////////////////////////////
VkSurfaceKHR glfw_window::create_vulkan_surface() const {
  VkSurfaceKHR result;
  vulkan::context &vulkan_context =
      vulkan::layer_abstraction_factory::instance().get_vulkan_context();
  auto &vulkan_instance = vulkan_context.mutable_vulkan();

  glfwCreateWindowSurface(vulkan_instance.get_instance(), m_window, nullptr,
                          &result);

  return result;
}

/////////////////////////////////////////////////////////////////////////////////////////
void glfw_window::init_input_event_listeners() const {
  glfwSetKeyCallback(m_window, [](GLFWwindow */*window*/, int key, int /*scanCode*/,
                                  int action, int /*mods*/) {
    switch (action) {
      case GLFW_PRESS:
      case GLFW_REPEAT: {
        event::keyboard::pressed press_event(
            static_cast<keyboard::key_code>(key));

        event_controller::on_event(press_event);
        break;
      }
      case GLFW_RELEASE: {
        event::keyboard::released release_event(
            static_cast<keyboard::key_code>(key));
        event_controller::on_event(release_event);

        break;
      }
      default:
        break;
    }
  });

  glfwSetCharCallback(m_window, [](GLFWwindow */*window*/, unsigned int key_code) {
    event::keyboard::symbol_pressed press_event(
        static_cast<keyboard::key_code>(key_code));

    event_controller::on_event(press_event);
  });

  glfwSetScrollCallback(
      m_window, [](GLFWwindow */*window*/, double x_offset, double y_offset) {
        wunder::event::mouse::scroll mouse_scrolled_event(
            glm::vec2(x_offset, y_offset));
        event_controller::on_event(mouse_scrolled_event);
      });

  glfwSetCursorPosCallback(
      m_window, [](GLFWwindow */*window*/, double x_pos, double y_pos) {
        wunder::event::mouse::move mouse_moved_event(glm::vec2(x_pos, y_pos));
        event_controller::on_event(mouse_moved_event);
      });

  glfwSetMouseButtonCallback(
      m_window, [](GLFWwindow */*window*/, int button, int action, int /*mods*/) {
        switch (action) {
          case GLFW_PRESS: {
            event::mouse::pressed mouse_pressed_event(
                static_cast<wunder::mouse::key_code>(button));
            event_controller::on_event(mouse_pressed_event);

            break;
          }
          case GLFW_RELEASE: {
            event::mouse::released mouse_released_event(
                static_cast<mouse::key_code>(button));
            event_controller::on_event(mouse_released_event);

            break;
          }
          default:
            break;
        }
      });
}
}  // namespace wunder
