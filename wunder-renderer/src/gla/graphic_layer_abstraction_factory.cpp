#include "gla/graphic_layer_abstraction_factory.h"

#include "core/wunder_macros.h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/vulkan_renderer.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

namespace wunder {
unique_ptr<graphic_layer_abstraction_factory>
    graphic_layer_abstraction_factory::s_instance = nullptr;

void graphic_layer_abstraction_factory::create_instance(
    const renderer_properties &properties) {
  switch (properties.m_renderer_type) {
    case gla_type::Vulkan: {
      s_instance = std::move(std::make_unique<vulkan_layer_abstraction_factory>());
    } break;
    default: {
      AssertReturnIf("Not handle graphic type.");
    }
  }

  s_instance->init_instance(properties);
}

graphic_layer_abstraction_factory &
graphic_layer_abstraction_factory::get_instance() {
  // review this later
  return *s_instance.get();
}

void graphic_layer_abstraction_factory::init_instance(
    const renderer_properties &properties) {
  init_instance_internal(properties);
}
}  // namespace wunder