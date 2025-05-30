#ifndef WUNDER_VULKAN_CONTEXT_H
#define WUNDER_VULKAN_CONTEXT_H

#include "core/non_copyable.h"
#include "core/wunder_memory.h"

//forward declarations
namespace wunder {
struct renderer_properties;
struct renderer_capabilities;
}  // namespace wunder

namespace wunder::vulkan {
class instance;
class physical_device;
class device;
class layer_abstraction_factory;
class swap_chain;

struct vulkan_extensions;
class memory_allocator;
}  // namespace wunder::vulkan

namespace wunder::vulkan {

class context final : public non_copyable {
 private:
  friend layer_abstraction_factory;

 public:
  context();
  ~context() override;

 private:
  void shutdown();
  void init(const renderer_properties& properties);

 public:
  [[nodiscard]] const renderer_capabilities& get_capabilities() const;

  [[nodiscard]] instance& mutable_vulkan();
  [[nodiscard]] physical_device& mutable_physical_device();
  [[nodiscard]] device& mutable_device();
  [[nodiscard]] swap_chain& mutable_swap_chain();
  [[nodiscard]] memory_allocator& mutable_resource_allocator();

 private:
  void create_vulkan_instance(const renderer_properties& properties);
  void select_physical_device();
  void select_logical_device();
  void create_allocator();
  void create_swap_chain(const renderer_properties& properties);

 private:
  unique_ptr<instance> m_vulkan;
  unique_ptr<physical_device> m_physical_device;
  unique_ptr<device> m_logical_device;
  unique_ptr<swap_chain> m_swap_chain;
  unique_ptr<memory_allocator> m_resource_allocator;

  unique_ptr<renderer_capabilities> m_renderer_capabilities;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_CONTEXT_H
