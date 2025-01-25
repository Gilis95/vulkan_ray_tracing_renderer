#ifndef WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H
#define WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H

#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "gla/renderer_properties.h"

namespace wunder::vulkan {
class rtx_renderer;
class context;

class layer_abstraction_factory {
 private:
  layer_abstraction_factory();

 public:
  ~layer_abstraction_factory();
  void shutdown();

 public:
  static layer_abstraction_factory &instance();

 public:
  void initialize(const renderer_properties &properties);

  [[nodiscard]] optional_ref<rtx_renderer> get_renderer_api(
      renderer_type type);
  [[nodiscard]] vector_map<renderer_type, unique_ptr<rtx_renderer>> &
  get_renderers();

  [[nodiscard]] context &get_vulkan_context();

 private:
  void create_renderer(const renderer_properties &properties);
  void create_vulkan_context(const renderer_properties &properties);

 private:
  vector_map<renderer_type, unique_ptr<rtx_renderer>> m_renderers;
  unique_ptr<context> m_context;
};

}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H
