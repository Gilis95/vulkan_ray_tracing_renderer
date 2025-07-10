#ifndef IMGUI_PANEL_H
#define IMGUI_PANEL_H

#include <imgui.h>

#include <string>

#include "glm/vec2.hpp"
#include "imgui_internal.h"

namespace imgui_h {
class panel {
  static ImGuiID s_dockspace_id;

 public:
  // Side where the panel will be
  enum class side {
    left,
    right,
  };

 public:
  // Starting the panel, equivalent to ImGui::Begin for a window. Need
  // ImGui::end()
  static void begin(side side = side::right, glm::vec2 dimensions = {320.f, 180.f}, float alpha = 0.5f,
                    const std::string& dock_name = "Settings");

  // Mirror begin but can use directly End()
  static void end() { ImGui::End(); }

  // Return the position and size of the central display
  static void central_dimension(ImVec2& pos, ImVec2& size) {
    auto dock_main = ImGui::DockBuilderGetCentralNode(s_dockspace_id);
    if (dock_main) {
      pos = dock_main->Pos;
      size = dock_main->Size;
    }
  }
};
}  // namespace imgui
#endif  // IMGUI_PANEL_H
