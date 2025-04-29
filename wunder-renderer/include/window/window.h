#ifndef WINDOW_H
#define WINDOW_H

#include <cstdint>
#include <vector>

#include "core/time_unit.h"

#include <glad/vulkan.h>

namespace wunder::vulkan{

struct vulkan_extensions {
 public:
  std::vector<const char *> m_extensions;
};

}

namespace wunder {
struct window_properties;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class window {
 public:
  window() = default;
  virtual ~window() = default;

 public:
  virtual void init(const window_properties &) = 0;

  virtual void update(time_unit dt) = 0;

  virtual void shutdown() = 0;

  virtual void fill_vulkan_extensions(
      wunder::vulkan::vulkan_extensions & out_extensions) const = 0;

  [[nodiscard]] virtual VkSurfaceKHR create_vulkan_surface() const = 0;

};
}  // namespace wunder

#endif
