#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_WINDOW_WINDOW_PROPERTIES_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_WINDOW_WINDOW_PROPERTIES_H_
#include <cstdint>
#include <string>

namespace wunder {
enum class window_type : std::int32_t { glfw };

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
struct window_properties {
 public:
  std::string m_title;
  std::uint32_t m_width;
  std::uint32_t m_height;
  window_type m_type;

  window_properties(std::string title = "wunder application",
                    std::uint32_t width = 1280, std::uint32_t height = 720,
                    window_type type = window_type::glfw)
      : m_title(std::move(title)),
        m_width(width),
        m_height(height),
        m_type(type) {}
};
}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_WINDOW_WINDOW_PROPERTIES_H_
