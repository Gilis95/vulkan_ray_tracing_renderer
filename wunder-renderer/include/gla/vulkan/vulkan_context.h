#ifndef WUNDER_VULKAN_CONTEXT_H
#define WUNDER_VULKAN_CONTEXT_H

#include "core/non_copyable.h"
#include "core/wunder_memory.h"

namespace wunder {
class vulkan;
class vulkan_physical_device;
class vulkan_device;
class vulkan_layer_abstraction_factory;
struct vulkan_extensions;
class vulkan_memory_allocator;

struct renderer_properties;
struct renderer_capabilities;

class vulkan_context final : public non_copyable {
 private:
  friend vulkan_layer_abstraction_factory;

 public:
  vulkan_context();
  ~vulkan_context();

 private:
  void init(const renderer_properties& properties);

 public:
  [[nodiscard]] const renderer_capabilities& get_capabilities() const;

  [[nodiscard]] vulkan& get_vulkan();
  [[nodiscard]] vulkan_physical_device& get_physical_device();
  [[nodiscard]] vulkan_device& get_device();
  [[nodiscard]] vulkan_memory_allocator& get_resource_allocator();

 private:
  void create_vulkan_instance(const renderer_properties& properties);
  void select_physical_device();
  void select_logical_device();
  void create_allocator();

 private:
  unique_ptr<vulkan> m_vulkan;
  unique_ptr<vulkan_physical_device> m_physical_device;
  unique_ptr<vulkan_device> m_logical_device;
  unique_ptr<vulkan_memory_allocator> m_resource_allocator;

  unique_ptr<renderer_capabilities> m_renderer_capabilities;
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_CONTEXT_H
