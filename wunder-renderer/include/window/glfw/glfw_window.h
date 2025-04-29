#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

#include <utility>

#include "window/window.h"
#include <glad/vulkan.h>

struct GLFWwindow;

namespace wunder {

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class glfw_window : public window {
 public:
  glfw_window();
  ~glfw_window() override;

 public:
  void init(const window_properties& properties) override;
  void update(time_unit dt) override;
  void shutdown() override;

 public:  // glfw callbacks
  static void on_close(GLFWwindow* window);

 public:
  void fill_vulkan_extensions(
      wunder::vulkan::vulkan_extensions& out_extensions) const override;

  VkSurfaceKHR create_vulkan_surface() const override;
 private:
  GLFWwindow* m_window;
  void init_input_event_listeners() const;
};
}  // namespace wunder

#endif
