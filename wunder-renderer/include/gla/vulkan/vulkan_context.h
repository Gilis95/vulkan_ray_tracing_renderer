#ifndef WUNDER_VULKAN_CONTEXT_H
#define WUNDER_VULKAN_CONTEXT_H

#include "core/non_copyable.h"
#include "core/wunder_memory.h"

namespace wunder {
struct renderer_properties;
struct renderer_capabilities;
}  // namespace wunder

namespace wunder::vulkan {
class instance;
class physical_device;
class device;
class layer_abstraction_factory;
struct vulkan_extensions;
class memory_allocator;

class context final : public non_copyable {
 private:
  friend layer_abstraction_factory;

 public:
  context();
  ~context();

 private:
  void init(const renderer_properties& properties);

 public:
  [[nodiscard]] const renderer_capabilities& get_capabilities() const;

  [[nodiscard]] instance& get_vulkan();
  [[nodiscard]] physical_device& get_physical_device();
  [[nodiscard]] device& get_device();
  [[nodiscard]] memory_allocator& get_resource_allocator();

 private:
  void create_vulkan_instance(const renderer_properties& properties);
  void select_physical_device();
  void select_logical_device();
  void create_allocator();

 private:
  unique_ptr<instance> m_vulkan;
  unique_ptr<physical_device> m_physical_device;
  unique_ptr<device> m_logical_device;
  unique_ptr<memory_allocator> m_resource_allocator;

  unique_ptr<renderer_capabilities> m_renderer_capabilities;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_CONTEXT_H
