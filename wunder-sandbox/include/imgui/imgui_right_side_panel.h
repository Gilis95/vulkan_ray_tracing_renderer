#ifndef RIGHT_SIDE_PANEL_H
#define RIGHT_SIDE_PANEL_H
#include "glm/vec2.hpp"

namespace wunder {
class time_unit;
}

namespace wunder {
class right_side_panel {
 public:
  static glm::vec2 s_dimensions;

 public:
  void update(time_unit dt);

 private:
  static void camera_tab_bar();
  static void camera_tab();

  void renderer_tab_bar();
  void renderer_tab();

};
}  // namespace wunder

#endif  // RIGHT_SIDE_PANEL_H
