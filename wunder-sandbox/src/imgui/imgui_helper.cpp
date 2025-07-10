#include "imgui/imgui_helper.h"

#include "imgui_internal.h"

namespace {
template <typename T>
bool clamped(bool changed, T* value, T min, T max) {
  *value = std::max(min, std::min(max, *value));
  return changed;
}
}  // namespace

namespace imgui_h::property_editor {

// Beginning the Property Editor
void begin(const char* label, ImGuiTableFlags flag) {
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
  const bool table_valid = ImGui::BeginTable(label, 2, flag);
  assert(table_valid);
}

// Generic entry, the lambda function should return true if the widget changed
bool entry(const std::string& property_name,
           const std::function<bool()>& content_fct,
           const std::string& tooltip) {
  ImGui::PushID(property_name.c_str());
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::AlignTextToFramePadding();
  ImGui::Text("%s", property_name.c_str());
  if (!tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal |
                                               ImGuiHoveredFlags_NoSharedDelay))
    ImGui::SetTooltip("%s", tooltip.c_str());
  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-FLT_MIN);
  bool result = content_fct();
  ImGui::PopID();
  return result;  // returning if the widget changed
}

// Text specialization
void entry(const std::string& property_name, const std::string& value) {
  entry(
      property_name,
      [&] {
        ImGui::Text("%s", value.c_str());
        return false;  // dummy, no change
      },
      "");
}

bool tree_node(const std::string& name) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::AlignTextToFramePadding();
  return ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
}

void treePop() { ImGui::TreePop(); }

// Ending the Editor
void end() {
  ImGui::EndTable();
  ImGui::PopStyleVar();
}

bool button(const char* label, const ImVec2& size, const std::string& tooltip) {
  return property_editor::entry(
      label, [&] { return ImGui::Button("##hidden", size); }, tooltip);
}

bool small_button(const char* label, const std::string& tooltip) {
  return entry(label, [&] { return ImGui::SmallButton("##hidden"); }, tooltip);
}

bool checkbox(const char* label, bool* v, const std::string& tooltip) {
  return entry(label, [&] { return ImGui::Checkbox("##hidden", v); }, tooltip);
}

bool radio_button(const char* label, bool active, const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::RadioButton("##hidden", active); }, tooltip);
}

bool combo(const char* label, int* current_item, const char* const items[],
           int items_count, int popup_max_height_in_items,
           const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::Combo("##hidden", current_item, items, items_count,
                            popup_max_height_in_items);
      },
      "");
}

bool combo(const char* label, int* current_item,
           const char* items_separated_by_zeros, int popup_max_height_in_items,
           const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::Combo("##hidden", current_item, items_separated_by_zeros,
                            popup_max_height_in_items);
      },
      tooltip);
}

bool combo(const char* label, int* current_item,
           const char* (*getter)(void* user_data, int idx), void* user_data,
           int items_count, int popup_max_height_in_items,
           const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::Combo("##hidden", current_item, getter, user_data,
                            items_count, popup_max_height_in_items);
      },
      tooltip);
}

bool slider_float(const char* label, float* v, float v_min, float v_max,
                  const char* format, ImGuiSliderFlags flags,
                  const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderFloat("##hidden", v, v_min, v_max, format, flags);
      },
      tooltip);
}

bool slider_float2(const char* label, float v[2], float v_min, float v_max,
                   const char* format, ImGuiSliderFlags flags,
                   const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderFloat2("##hidden", v, v_min, v_max, format, flags);
      },
      tooltip);
}

bool slider_float3(const char* label, float v[3], float v_min, float v_max,
                   const char* format, ImGuiSliderFlags flags,
                   const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderFloat3("##hidden", v, v_min, v_max, format, flags);
      },
      tooltip);
}

bool slider_float4(const char* label, float v[4], float v_min, float v_max,
                   const char* format, ImGuiSliderFlags flags,
                   const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderFloat4("##hidden", v, v_min, v_max, format, flags);
      },
      tooltip);
}

bool slider_angle(const char* label, float* v_rad, float v_degrees_min,
                  float v_degrees_max, const char* format,
                  ImGuiSliderFlags flags, const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderAngle("##hidden", v_rad, v_degrees_min,
                                  v_degrees_max, format, flags);
      },
      tooltip);
}

bool slider_int(const char* label, int* v, int v_min, int v_max,
                const char* format, ImGuiSliderFlags flags,
                const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderInt("##hidden", v, v_min, v_max, format, flags);
      },
      tooltip);
}

bool slider_int2(const char* label, int v[2], int v_min, int v_max,
                 const char* format, ImGuiSliderFlags flags,
                 const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderInt2("##hidden", v, v_min, v_max, format, flags);
      },
      tooltip);
}

bool slider_int3(const char* label, int v[3], int v_min, int v_max,
                 const char* format, ImGuiSliderFlags flags,
                 const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderInt3("##hidden", v, v_min, v_max, format, flags);
      },
      tooltip);
}

bool slider_int4(const char* label, int v[4], int v_min, int v_max,
                 const char* format, ImGuiSliderFlags flags,
                 const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderInt4("##hidden", v, v_min, v_max, format, flags);
      },
      tooltip);
}

bool slider_scalar(const char* label, ImGuiDataType data_type, void* p_data,
                   const void* p_min, const void* p_max, const char* format,
                   ImGuiSliderFlags flags, const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::SliderScalar("##hidden", data_type, p_data, p_min, p_max,
                                   format, flags);
      },
      tooltip);
}

bool drag_float(const char* label, float* v, float v_speed, float v_min,
                float v_max, const char* format, ImGuiSliderFlags flags,
                const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::DragFloat("##hidden", v, v_speed, v_min, v_max, format,
                                flags);
      },
      tooltip);
}

bool drag_float2(const char* label, float v[2], float v_speed, float v_min,
                 float v_max, const char* format, ImGuiSliderFlags flags,
                 const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::DragFloat2("##hidden", v, v_speed, v_min, v_max, format,
                                 flags);
      },
      tooltip);
}

bool drag_float3(const char* label, float v[3], float v_speed, float v_min,
                 float v_max, const char* format, ImGuiSliderFlags flags,
                 const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::DragFloat3("##hidden", v, v_speed, v_min, v_max, format,
                                 flags);
      },
      tooltip);
}

bool drag_float4(const char* label, float v[4], float v_speed, float v_min,
                 float v_max, const char* format, ImGuiSliderFlags flags,
                 const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::DragFloat4("##hidden", v, v_speed, v_min, v_max, format,
                                 flags);
      },
      tooltip);
}

bool drag_int(const char* label, int* v, float v_speed, int v_min, int v_max,
              const char* format, ImGuiSliderFlags flags,
              const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::DragInt("##hidden", v, v_speed, v_min, v_max, format,
                              flags);
      },
      tooltip);
}

bool drag_int2(const char* label, int v[2], float v_speed, int v_min, int v_max,
               const char* format, ImGuiSliderFlags flags,
               const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::DragInt2("##hidden", v, v_speed, v_min, v_max, format,
                               flags);
      },
      tooltip);
}

bool drag_int3(const char* label, int v[3], float v_speed, int v_min, int v_max,
               const char* format, ImGuiSliderFlags flags,
               const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::DragInt3("##hidden", v, v_speed, v_min, v_max, format,
                               flags);
      },
      tooltip);
}

bool drag_int4(const char* label, int v[4], float v_speed, int v_min, int v_max,
               const char* format, ImGuiSliderFlags flags,
               const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::DragInt4("##hidden", v, v_speed, v_min, v_max, format,
                               flags);
      },
      tooltip);
}

bool drag_scalar(const char* label, ImGuiDataType data_type, void* p_data,
                 float v_speed /*= 1.0f*/, const void* p_min /*= NULL*/,
                 const void* p_max /*= NULL*/, const char* format /*= NULL*/,
                 ImGuiSliderFlags flags /*= 0*/,
                 const std::string& tooltip /*= {}*/) {
  return entry(
      label,
      [&] {
        return ImGui::DragScalar("##hidden", data_type, p_data, v_speed, p_min,
                                 p_max, format, flags);
      },
      tooltip);
}

bool input_text(const char* label, char* buf, size_t buf_size,
                ImGuiInputTextFlags flags, const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::InputText("##hidden", buf, buf_size, flags); },
      tooltip);
}

bool input_text_multiline(const char* label, char* buf, size_t buf_size,
                          const ImVec2& size, ImGuiInputTextFlags flags,
                          const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::InputTextMultiline("##hidden", buf, buf_size, size,
                                         flags);
      },
      tooltip);
}

bool input_float(const char* label, float* v, float step, float step_fast,
                 const char* format, ImGuiInputTextFlags flags,
                 const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::InputFloat("##hidden", v, step, step_fast, format, flags);
      },
      tooltip);
}

bool input_float2(const char* label, float v[2], const char* format,
                  ImGuiInputTextFlags flags, const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::InputFloat2("##hidden", v, format, flags); },
      tooltip);
}

bool input_float3(const char* label, float v[3], const char* format,
                  ImGuiInputTextFlags flags, const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::InputFloat3("##hidden", v, format, flags); },
      tooltip);
}

bool input_float4(const char* label, float v[4], const char* format,
                  ImGuiInputTextFlags flags, const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::InputFloat4("##hidden", v, format, flags); },
      tooltip);
}

bool input_int(const char* label, int* v, int step, int step_fast,
               ImGuiInputTextFlags flags, const std::string& tooltip) {
  return entry(
      label,
      [&] { return ImGui::InputInt("##hidden", v, step, step_fast, flags); },
      tooltip);
}

bool input_int_clamped(const char* label, int* v, int min, int max, int step,
                       int step_fast, ImGuiInputTextFlags flags,
                       const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return clamped(ImGui::InputInt("##hidden", v, step, step_fast, flags),
                       v, min, max);
      },
      tooltip);
}

bool input_int2(const char* label, int v[2], ImGuiInputTextFlags flags,
                const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::InputInt2("##hidden", v, flags); }, tooltip);
}

bool input_int3(const char* label, int v[3], ImGuiInputTextFlags flags,
                const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::InputInt3("##hidden", v, flags); }, tooltip);
}

bool input_int4(const char* label, int v[4], ImGuiInputTextFlags flags,
                const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::InputInt4("##hidden", v, flags); }, tooltip);
}

bool input_double(const char* label, double* v, double step, double step_fast,
                  const char* format, ImGuiInputTextFlags flags,
                  const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::InputDouble("##hidden", v, step, step_fast, format,
                                  flags);
      },
      tooltip);
}

bool input_scalar(const char* label, ImGuiDataType data_type, void* p_data,
                  const void* p_step, const void* p_step_fast,
                  const char* format, ImGuiInputTextFlags flags,
                  const std::string& tooltip) {
  return entry(
      label,
      [&] {
        return ImGui::InputScalar("##hidden", data_type, p_data, p_step,
                                  p_step_fast, format, flags);
      },
      tooltip);
}

bool color_edit3(const char* label, float col[3], ImGuiColorEditFlags flags,
                 const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::ColorEdit3("##hidden", col, flags); },
      tooltip);
}
bool color_edit4(const char* label, float col[4], ImGuiColorEditFlags flags,
                 const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::ColorEdit4("##hidden", col, flags); },
      tooltip);
}

bool color_picker3(const char* label, float col[3], ImGuiColorEditFlags flags,
                   const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::ColorPicker3("##hidden", col, flags); },
      tooltip);
}

bool color_picker4(const char* label, float col[4], ImGuiColorEditFlags flags,
                   const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::ColorPicker4("##hidden", col, flags); },
      tooltip);
}

bool color_button(const char* label, const ImVec4& col,
                  ImGuiColorEditFlags flags, const ImVec2& size,
                  const std::string& tooltip) {
  return entry(
      label, [&] { return ImGui::ColorButton("##hidden", col, flags, size); },
      tooltip);
}

bool text(const char* label, const std::string& text) {
  return entry(
      label,
      [&] {
        ImGui::Text("%s", text.c_str());
        return false;  // dummy, no change
      },
      "");
}

bool text(const char* label, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  bool res = entry(
      label,
      [&] {
        ImGui::TextV(fmt, args);
        return false;  // dummy, no change
      },
      "");
  va_end(args);
  return res;
}

void show_property_label(const std::string& text,
                         const std::string& description) {
  if (text.back() == '\n') {
    ImGui::TextWrapped("%s", text.c_str());
  } else {
    // Indenting the text to be right justified
    float current_indent = ImGui::GetCursorPos().x;
    const ImGuiStyle& imstyle = ImGui::GetStyle();

    float control_width = std::min(
        (ImGui::GetWindowWidth() - imstyle.IndentSpacing) * 0.7f, 500.f);
    if (ImGui::GetCurrentContext() &&
        ImGui::GetCurrentContext()->CurrentWindow) {
      control_width -=
          ImGui::GetCurrentContext()->CurrentWindow->ScrollbarSizes.x;
    }
    control_width = std::max(control_width, 50.f);

    float available_width = ImGui::GetContentRegionAvail().x;
    float avaiable_text_width =
        available_width - control_width - imstyle.ItemInnerSpacing.x;
    ImVec2 text_size = ImGui::CalcTextSize(
        text.c_str(), text.c_str() + text.size(), false, avaiable_text_width);
    float indent = current_indent + available_width - control_width -
                   text_size.x - imstyle.ItemInnerSpacing.x;

    ImGui::AlignTextToFramePadding();
    ImGui::NewLine();
    ImGui::SameLine(indent);
    ImGui::PushTextWrapPos(indent + avaiable_text_width);
    ImGui::TextWrapped("%s", text.c_str());
    ImGui::PopTextWrapPos();
    ImGui::SameLine();
  }

  ImGui::SetTooltip("%s", description.c_str());
}

bool show_numeric_control(const std::string& /*label*/,
                          const std::string& /*description*/, std::int32_t* value,
                          const std::int32_t* default_value,
                          std::function<bool(void)> show_numeric_control) {
  ImGui::PushID(value);
  // show_property_label(label, description);

  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  bool changed = show_numeric_control();

  if (default_value && ImGui::BeginPopupContextItem("item context menu")) {
    if (ImGui::Selectable("set default")) {
      changed = *value != *default_value;
      *value = *default_value;
    }
    ImGui::EndPopup();
  }

  ImGui::PopID();
  return changed;
}

bool selection(const std::string& label, const std::string& description,
               std::int32_t* index, std::int32_t* default_index,
               std::function<const char*(std::int32_t)> get_value) {
  return show_numeric_control(label, description, index, default_index, [&] {
    bool valid = false;
    bool changed = false;
    if (ImGui::BeginCombo("##hidden", get_value(*index))) {
      std::int32_t i = 0;
      while (true) {
        const char* option = get_value(i);
        if (!option) break;

        valid |= (i == *index);  // check if current selection is a valid option
        if (ImGui::Selectable(option, i == *index)) {
          *index = i;
          changed = true;
          valid = true;
        }
        ++i;
      }
      ImGui::EndCombo();

      if (!valid && default_index) {
        *index = *default_index;
        changed = true;
      }
    }
    return changed;
  });
}

bool selection(const std::string& label, const std::string& description,
                      std::int32_t* index, std::int32_t* default_index,
                      const std::vector<std::string>& values) {
  return selection(label, description, index, default_index,
                   [&](std::int32_t i) {
                     return i < values.size() ? values[i].c_str() : nullptr;
                   });
}

}  // namespace imgui_h::property_editor
   // namespace imgui