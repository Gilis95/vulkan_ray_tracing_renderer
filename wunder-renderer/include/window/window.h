#ifndef WINDOW_H
#define WINDOW_H

#include <glad/vulkan.h>

#include <cstdint>
#include <vector>

#include "core/time_unit.h"
#include "core/wunder_macros.h"
#include "window/window_properties.h"

namespace wunder::vulkan {

struct vulkan_extensions {
 public:
  std::vector<const char *> m_extensions;
};

}  // namespace wunder::vulkan

namespace wunder {
struct window_properties;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class window {
 public:
  explicit window(window_type type) : m_type(type) {}
  virtual ~window() = default;

 public:
  virtual void init(const window_properties &) = 0;

  virtual void update(time_unit dt) = 0;

  virtual void fill_vulkan_extensions(
      wunder::vulkan::vulkan_extensions &out_extensions) const = 0;

  [[nodiscard]] virtual VkSurfaceKHR create_vulkan_surface() const = 0;

  template<derived<window> window_sub_type>
  [[nodiscard]] constexpr optional_ref<window_sub_type> as() {
    if (window_sub_type::type == type()) {
      return *static_cast<window_sub_type *>(this);
    }

    return std::nullopt;
  }

 [[nodiscard]] window_type type() const { return m_type; }
private:
  window_type m_type;
};
}  // namespace wunder

#endif
