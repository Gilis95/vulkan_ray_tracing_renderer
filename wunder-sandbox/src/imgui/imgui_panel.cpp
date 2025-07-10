#include "imgui/imgui_panel.h"

#include <string>

#include "glm/common.hpp"
#include "window_helper.h"

namespace imgui_h {
// Static member declaration
ImGuiID panel::s_dockspace_id{0};

void panel::begin(side side, glm::vec2 dimensions, float alpha,
                  const std::string& dock_name) {
  // Keeping the unique ID of the dock space
  s_dockspace_id = ImGui::GetID("DockSpace");

  // The dock need a dummy window covering the entire viewport.
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);

  // All flags to dummy window
  ImGuiWindowFlags host_window_flags{};
  host_window_flags |= ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
  host_window_flags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
  host_window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  host_window_flags |= ImGuiWindowFlags_NoBackground;

  // Starting dummy window
  char label[32];
  ImFormatString(label, IM_ARRAYSIZE(label), "DockSpaceViewport_%08X",
                 viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin(label, nullptr, host_window_flags);
  ImGui::PopStyleVar(3);

  // The central node is transparent, so that when UI is draw after, the image
  // is visible Auto Hide Bar, no title of the panel Center is not dockable,
  // that is for the scene
  ImGuiDockNodeFlags dockspace_flags =
      ImGuiDockNodeFlags_PassthruCentralNode |
      ImGuiDockNodeFlags_AutoHideTabBar |
      ImGuiDockNodeFlags_NoDockingOverCentralNode;

  // Building the splitting of the dock space is done only once
  if (!ImGui::DockBuilderGetNode(s_dockspace_id)) {
    ImGui::DockBuilderRemoveNode(s_dockspace_id);
    ImGui::DockBuilderAddNode(s_dockspace_id,
                              dockspace_flags | ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(s_dockspace_id, viewport->Size);

    ImGuiID dock_main_id = s_dockspace_id;

    // Slitting all 4 directions, targetting (320 pixel * DPI) panel width, (180
    // pixel * DPI) panel height.
    const float xRatio = glm::clamp<float>(
        dimensions.x * wunder::get_dpi_scale() / viewport->WorkSize[0], 0.01f,
        0.499f);
    const float yRatio = glm::clamp<float>(
        dimensions.y * wunder::get_dpi_scale() / viewport->WorkSize[1], 0.01f,
        0.499f);
    ImGuiID id_left, id_right, id_up, id_down;

    // Note, for right, down panels, we use the n / (1 - n) formula to correctly
    // split the space remaining from the left, up panels.
    id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, xRatio,
                                          nullptr, &dock_main_id);
    id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right,
                                           xRatio / (1 - xRatio), nullptr,
                                           &dock_main_id);
    id_up = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, yRatio,
                                        nullptr, &dock_main_id);
    id_down = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down,
                                          yRatio / (1 - yRatio), nullptr,
                                          &dock_main_id);

    ImGui::DockBuilderDockWindow(
        side == side::left ? dock_name.c_str() : "Dock_left", id_left);
    ImGui::DockBuilderDockWindow(
        side == side::right ? dock_name.c_str() : "Dock_right", id_right);
    ImGui::DockBuilderDockWindow("Dock_up", id_up);
    ImGui::DockBuilderDockWindow("Dock_down", id_down);
    ImGui::DockBuilderDockWindow("Scene", dock_main_id);  // Center

    ImGui::DockBuilderFinish(dock_main_id);
  }

  // Setting the panel to blend with alpha
  ImVec4 col = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(col.x, col.y, col.z, alpha));

  ImGui::DockSpace(s_dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  ImGui::PopStyleColor();
  ImGui::End();

  // The panel
  if (alpha < 1)
    ImGui::SetNextWindowBgAlpha(
        alpha);  // For when the panel becomes a floating window
  ImGui::Begin(dock_name.c_str());
}
}  // namespace imgui_h