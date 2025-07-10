#ifndef WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H
#define WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H

#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "gla/renderer_properties.h"

namespace wunder {
struct application_properties;
}
namespace wunder::vulkan {
class renderer_context;
}
namespace wunder::vulkan {
class rtx_renderer;
class context;

class layer_abstraction_factory {
 private:
  layer_abstraction_factory();

 public:
  ~layer_abstraction_factory();
  void begin_shutdown();
  void end_shutdown();

 public:
  static layer_abstraction_factory &instance();

 public:
  void init(const application_properties &properties);

  [[nodiscard]] renderer_context &get_render_context();

  [[nodiscard]] context &get_vulkan_context();

 private:
  void create_renderer(const application_properties &properties);
  void create_vulkan_context(const renderer_properties &properties);

 private:
  unique_ptr<renderer_context> m_renderer_context;
  unique_ptr<context> m_context;
};

}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H
