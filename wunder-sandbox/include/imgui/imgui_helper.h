#ifndef IMGUI_HELPER_H
#define IMGUI_HELPER_H
#include <cstdint>
#include <functional>
#include <string>

#include "imgui.h"

namespace imgui_h {
namespace property_editor {
void begin(const char* label = "PE::Table",
           ImGuiTableFlags flag = ImGuiTableFlags_BordersOuter |
                                  ImGuiTableFlags_Resizable);
void end();
inline bool entry(const std::string& property_name,
                  const std::function<bool()>& content_fct,
                  const std::string& tooltip = {});
void entry(const std::string& property_name, const std::string& value);
bool tree_node(const std::string& name);
void treePop();

bool button(const char* label, const ImVec2& size = ImVec2(0, 0),
            const std::string& tooltip = {});
bool small_button(const char* label, const std::string& tooltip = {});
bool checkbox(const char* label, bool* v, const std::string& tooltip = {});
bool radio_button(const char* label, bool active,
                  const std::string& tooltip = {});
bool combo(const char* label, int* current_item, const char* const items[],
           int items_count, int popup_max_height_in_items = -1,
           const std::string& tooltip = {});
bool combo(const char* label, int* current_item,
           const char* items_separated_by_zeros,
           int popup_max_height_in_items = -1, const std::string& tooltip = {});
bool combo(const char* label, int* current_item,
           const char* (*getter)(void* user_data, int idx), void* user_data,
           int items_count, int popup_max_height_in_items = -1,
           const std::string& tooltip = {});
bool slider_float(const char* label, float* v, float v_min, float v_max,
                  const char* format = "%.3f", ImGuiSliderFlags flags = 0,
                  const std::string& tooltip = {});
bool slider_float2(const char* label, float v[2], float v_min, float v_max,
                   const char* format = "%.3f", ImGuiSliderFlags flags = 0,
                   const std::string& tooltip = {});
bool slider_float3(const char* label, float v[3], float v_min, float v_max,
                   const char* format = "%.3f", ImGuiSliderFlags flags = 0,
                   const std::string& tooltip = {});
bool slider_float4(const char* label, float v[4], float v_min, float v_max,
                   const char* format = "%.3f", ImGuiSliderFlags flags = 0,
                   const std::string& tooltip = {});
bool slider_angle(const char* label, float* v_rad,
                  float v_degrees_min = -360.0f, float v_degrees_max = +360.0f,
                  const char* format = "%.0f deg", ImGuiSliderFlags flags = 0,
                  const std::string& tooltip = {});
bool slider_int(const char* label, int* v, int v_min, int v_max,
                const char* format = "%d", ImGuiSliderFlags flags = 0,
                const std::string& tooltip = {});
bool slider_int2(const char* label, int v[2], int v_min, int v_max,
                 const char* format = "%d", ImGuiSliderFlags flags = 0,
                 const std::string& tooltip = {});
bool slider_int3(const char* label, int v[3], int v_min, int v_max,
                 const char* format = "%d", ImGuiSliderFlags flags = 0,
                 const std::string& tooltip = {});
bool slider_int4(const char* label, int v[4], int v_min, int v_max,
                 const char* format = "%d", ImGuiSliderFlags flags = 0,
                 const std::string& tooltip = {});
bool slider_scalar(const char* label, ImGuiDataType data_type, void* p_data,
                   const void* p_min, const void* p_max,
                   const char* format = NULL, ImGuiSliderFlags flags = 0,
                   const std::string& tooltip = {});
bool drag_float(const char* label, float* v, float v_speed = 1.0f,
                float v_min = 0.0f, float v_max = 0.0f,
                const char* format = "%.3f", ImGuiSliderFlags flags = 0,
                const std::string& tooltip = {});
bool drag_float2(const char* label, float v[2], float v_speed = 1.0f,
                 float v_min = 0.0f, float v_max = 0.0f,
                 const char* format = "%.3f", ImGuiSliderFlags flags = 0,
                 const std::string& tooltip = {});
bool drag_float3(const char* label, float v[3], float v_speed = 1.0f,
                 float v_min = 0.0f, float v_max = 0.0f,
                 const char* format = "%.3f", ImGuiSliderFlags flags = 0,
                 const std::string& tooltip = {});
bool drag_float4(const char* label, float v[4], float v_speed = 1.0f,
                 float v_min = 0.0f, float v_max = 0.0f,
                 const char* format = "%.3f", ImGuiSliderFlags flags = 0,
                 const std::string& tooltip = {});
bool drag_int(const char* label, int* v, float v_speed = 1.0f, int v_min = 0,
              int v_max = 0, const char* format = "%d",
              ImGuiSliderFlags flags = 0, const std::string& tooltip = {});
bool drag_int2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0,
               int v_max = 0, const char* format = "%d",
               ImGuiSliderFlags flags = 0, const std::string& tooltip = {});
bool drag_int3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0,
               int v_max = 0, const char* format = "%d",
               ImGuiSliderFlags flags = 0, const std::string& tooltip = {});
bool drag_int4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0,
               int v_max = 0, const char* format = "%d",
               ImGuiSliderFlags flags = 0, const std::string& tooltip = {});
bool drag_scalar(const char* label, ImGuiDataType data_type, void* p_data,
                 float v_speed = 1.0f, const void* p_min = NULL,
                 const void* p_max = NULL, const char* format = NULL,
                 ImGuiSliderFlags flags = 0, const std::string& tooltip = {});
bool input_text(const char* label, char* buf, size_t buf_size,
                ImGuiInputTextFlags flags = 0, const std::string& tooltip = {});
bool input_text_multiline(const char* label, char* buf, size_t buf_size,
                          const ImVec2& size = ImVec2(0, 0),
                          ImGuiInputTextFlags flags = 0,
                          const std::string& tooltip = {});
bool input_float(const char* label, float* v, float step = 0.0f,
                 float step_fast = 0.0f, const char* format = "%.3f",
                 ImGuiInputTextFlags flags = 0,
                 const std::string& tooltip = {});
bool input_float2(const char* label, float v[2], const char* format = "%.3f",
                  ImGuiInputTextFlags flags = 0,
                  const std::string& tooltip = {});
bool input_float3(const char* label, float v[3], const char* format = "%.3f",
                  ImGuiInputTextFlags flags = 0,
                  const std::string& tooltip = {});
bool input_float4(const char* label, float v[4], const char* format = "%.3f",
                  ImGuiInputTextFlags flags = 0,
                  const std::string& tooltip = {});
bool input_int(const char* label, int* v, int step = 1, int step_fast = 100,
               ImGuiInputTextFlags flags = 0, const std::string& tooltip = {});
bool input_int_clamped(const char* label, int* v, int min, int max,
                       int step = 1, int step_fast = 100,
                       ImGuiInputTextFlags flags = 0,
                       const std::string& tooltip = {});
bool input_int2(const char* label, int v[2], ImGuiInputTextFlags flags = 0,
                const std::string& tooltip = {});
bool input_int3(const char* label, int v[3], ImGuiInputTextFlags flags = 0,
                const std::string& tooltip = {});
bool input_int4(const char* label, int v[4], ImGuiInputTextFlags flags = 0,
                const std::string& tooltip = {});
bool input_double(const char* label, double* v, double step = 0.0,
                  double step_fast = 0.0, const char* format = "%.6f",
                  ImGuiInputTextFlags flags = 0,
                  const std::string& tooltip = {});
bool input_scalar(const char* label, ImGuiDataType data_type, void* p_data,
                  const void* p_step = NULL, const void* p_step_fast = NULL,
                  const char* format = NULL, ImGuiInputTextFlags flags = 0,
                  const std::string& tooltip = {});
bool color_edit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0,
                 const std::string& tooltip = {});
bool color_edit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0,
                 const std::string& tooltip = {});
bool color_picker3(const char* label, float col[3],
                   ImGuiColorEditFlags flags = 0,
                   const std::string& tooltip = {});
bool color_picker4(const char* label, float col[4],
                   ImGuiColorEditFlags flags = 0,
                   const std::string& tooltip = {});
bool color_button(const char* label, const ImVec4& col,
                  ImGuiColorEditFlags flags = 0,
                  const ImVec2& size = ImVec2(0, 0),
                  const std::string& tooltip = {});
bool text(const char* label, const std::string& text);
bool text(const char* label, const char* fmt, ...);

bool selection(const std::string& label, const std::string& description,
               std::int32_t* index, std::int32_t* default_index,
               std::function<const char*(std::int32_t)> get_value);

bool selection(const std::string& label, const std::string& description,
               std::int32_t* index, std::int32_t* default_index,
               const std::vector<std::string>& values);
}  // namespace property_editor
}  // namespace imgui_h

#endif  // IMGUI_HELPER_H
