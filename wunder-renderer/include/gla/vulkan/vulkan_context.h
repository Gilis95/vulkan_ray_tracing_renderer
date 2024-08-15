//
// Created by christian on 8/9/24.
//

#ifndef WUNDER_VULKAN_CONTEXT_H
#define WUNDER_VULKAN_CONTEXT_H

#include "core/wunder_memory.h"

namespace wunder {
class vulkan;
class vulkan_physical_device;
class vulkan_device;
class vulkan_layer_abstraction_factory;
struct vulkan_extensions;

struct renderer_properties;
struct renderer_capabilities;

class vulkan_context final {
 private:
  friend vulkan_layer_abstraction_factory;
 public:
  vulkan_context();
  ~vulkan_context();
 private:
  void init(const renderer_properties& properties);
 public:
  [[nodiscard]] const renderer_capabilities& get_capabilities() const;
  [[nodiscard]] vulkan_device& get_device();
  [[nodiscard]] vulkan_physical_device& get_physical_device();
  [[nodiscard]] vulkan& get_vulkan();

 private:
  void create_vulkan_instance(const renderer_properties& properties);
  void select_logical_device();
  void select_physical_device();
 private:
  shared_ptr<vulkan> m_vulkan;
  unique_ptr<renderer_capabilities> m_renderer_capabilities;
  shared_ptr<vulkan_physical_device> m_physical_device;
  unique_ptr<vulkan_device> m_logical_device;
};
}
#endif  // WUNDER_VULKAN_CONTEXT_H