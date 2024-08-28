#ifndef WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H
#define WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H

#include "core/wunder_memory.h"

namespace wunder {
struct renderer_properties;

class vulkan_renderer;
class renderer_api;
class vulkan_context;

class vulkan_layer_abstraction_factory {
 private:
  vulkan_layer_abstraction_factory();
 public:
  ~vulkan_layer_abstraction_factory();
 public:
  static vulkan_layer_abstraction_factory &instance();
 public:
  void init_instance(const renderer_properties &properties);

  [[nodiscard]] vulkan_renderer &get_renderer_api();
  [[nodiscard]] vulkan_context &get_vulkan_context();
 private:
  void create_renderer(const renderer_properties &properties);
 private:
  unique_ptr<vulkan_renderer> m_renderer;
  unique_ptr<vulkan_context> m_context;
  void create_vulkan_context(const renderer_properties &properties);
};

}  // namespace wunder

#endif  // WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H
