#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

#include <glad/vulkan.h>

#include <utility>

#include "window/window.h"

struct GLFWwindow;

namespace wunder {

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class glfw_window : public window {
 public:
  static constexpr window_type type = window_type::glfw;

 public:
  glfw_window();
  ~glfw_window() override;

 public:
  void init(const window_properties& properties) override;
  void update(time_unit dt) override;

 public:  // glfw callbacks
  static void on_close(GLFWwindow* window);

 public:
  void fill_vulkan_extensions(
      wunder::vulkan::vulkan_extensions& out_extensions) const override;

  [[nodiscard]] VkSurfaceKHR create_vulkan_surface() const override;

 public:
  [[nodiscard]] GLFWwindow* mutable_window() { return m_window; }

 private:
  GLFWwindow* m_window;
  void init_input_event_listeners() const;
};
}  // namespace wunder

#endif
