#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

#include <utility>

#include "window/window.h"

class GLFWwindow;

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

 public:
  [[nodiscard]] vulkan_extension get_vulkan_extensions() const override;

 private:
  GLFWwindow *m_window;
};
}  // namespace wunder

#endif
