#ifndef WINDOW_H
#define WINDOW_H

#include <cstdint>

namespace wunder {
struct window_properties;

struct vulkan_extension {
 public:
  uint32_t m_extensions_count = 0;
  const char **m_extensions = nullptr;
};
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class window {
 public:
  window() = default;
  virtual ~window() = default;

 public:
  virtual void init(const window_properties &) = 0;

  virtual void update(int dt) = 0;

  virtual void shutdown() = 0;

  [[nodiscard]] virtual vulkan_extension get_vulkan_extensions() const = 0;
};
}  // namespace wunder

#endif
