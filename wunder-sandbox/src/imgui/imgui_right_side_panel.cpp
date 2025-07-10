#include "imgui/imgui_right_side_panel.h"

#include "camera/camera.h"
#include "core/services_factory.h"
#include "gla/vulkan/ray-trace/vulkan_rtx_renderer.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_renderer_context.h"
#include "imgui.h"
#include "imgui/imgui_helper.h"
#include "imgui/imgui_panel.h"

namespace wunder {

glm::vec2 right_side_panel::s_dimensions = {320.f, 180.f};

void right_side_panel::update(time_unit /*dt*/) {
  std::string panel_name = "Settings";
  imgui_h::panel::begin(imgui_h::panel::side::right, s_dimensions, 1.0f,
                        panel_name);

  camera_tab_bar();
  renderer_tab_bar();

  imgui_h::panel::end();
}

void right_side_panel::camera_tab_bar() {
  ReturnUnless(
      ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen));

  camera_tab();
}

void right_side_panel::camera_tab() {
  namespace PE = imgui_h::property_editor;

  bool changed = false;
  auto& camera = wunder::service_factory::instance().get_camera();
  auto& camera_properties = camera.mutable_camera_properties();

  PE::begin();
  PE::input_float3("Eye", &camera_properties.eye.x, "%.5f", 0,
                   "Position of the Camera");
  changed |= ImGui::IsItemDeactivatedAfterEdit();
  PE::input_float3("Center", &camera_properties.ctr.x, "%.5f", 0,
                   "Center of camera interest");
  changed |= ImGui::IsItemDeactivatedAfterEdit();
  PE::input_float3("Up", &camera_properties.up.x, "%.5f", 0,
                   "Up vector interest");
  changed |= ImGui::IsItemDeactivatedAfterEdit();

  PE::end();

  ReturnUnless(changed);
  camera.update_view_matrix();
}

void right_side_panel::renderer_tab_bar() {
  ReturnUnless(
      ImGui::CollapsingHeader("Rtx Renderer", ImGuiTreeNodeFlags_DefaultOpen));

  renderer_tab();
}

void right_side_panel::renderer_tab() {
  namespace PE = imgui_h::property_editor;

  auto& rtx_renderer = vulkan::layer_abstraction_factory::instance()
                           .get_render_context()
                           .mutable_rtx_renderer();
  auto& rtx_config = rtx_renderer.mutable_rtx_config();
  bool changed = false;

  PE::begin();
  changed |= PE::slider_int("Samples per pixel", &rtx_config.maxSamples, 1, 25);
  changed |= PE::slider_int("Samples depth", &rtx_config.maxDepth, 1, 25);
  // changed |=
  //     PE::selection("Debug Mode", "", &rtx_config.debugging_mode, nullptr,
  //                   {
  //                       "No Debug",
  //                       "BaseColor",
  //                       "Normal",
  //                       "Metallic",
  //                       "Emissive",
  //                       "Alpha",
  //                       "Roughness",
  //                       "TexCoord",
  //                       "Tangent",
  //                       "Radiance",
  //                       "Weight",
  //                       "RayDir",
  //                       "HeatMap",
  //                   });
  PE::end();

  ReturnUnless(changed);
  rtx_renderer.reset_frames();
}
}  // namespace wunder
