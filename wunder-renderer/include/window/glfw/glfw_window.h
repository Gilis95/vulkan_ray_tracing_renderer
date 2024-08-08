#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

#include <utility>

#include "window/window.h"

struct GLFWwindow;

namespace wunder {

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class glfw_window : public window {
 public:
  glfw_window();
  ~glfw_window() override;

 public:
  void init(const window_properties &properties) override;
  void update(int dt) override;
  void shutdown() override;
 public: // glfw callbacks
  static void on_close(GLFWwindow* window);
 public:
  [[nodiscard]] void fill_vulkan_extensions(
      vulkan_extensions& out_extensions) const override;

 private:
  GLFWwindow *m_window;
};
}  // namespace wunder

#endif
