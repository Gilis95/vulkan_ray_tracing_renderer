#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

#include <functional>
#include <initializer_list>
#include <memory>

#include "core/vector_map.h"
#include "gla/renderer_properties.h"
#include "gla/renderer_capabilities .h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_logical_device.h"
#include "gla/vulkan/vulkan_renderer.h"

namespace {
std::unique_ptr<wunder::vulkan_renderer> create_ray_trace_vulkan_render(
    const wunder::renderer_properties &properties) {
  auto renderer = std::make_unique<wunder::vulkan_renderer>();
  renderer->init(properties);

  return renderer;
}

using renderer_create_fn = std::function<std::unique_ptr<wunder::vulkan_renderer>(
   const wunder::renderer_properties &)>;

std::pair<wunder::renderer_type, renderer_create_fn>
    ray_tracing_renderer_mapping =
    std::make_pair<wunder::renderer_type, renderer_create_fn>(
        wunder::renderer_type::RAY_TRACE,
        std::bind(create_ray_trace_vulkan_render, std::placeholders::_1));

wunder::vector_map<wunder::renderer_type, renderer_create_fn>
    renderer_creation_fns(
    {ray_tracing_renderer_mapping});
}  // namespace

namespace wunder {
vulkan_layer_abstraction_factory::~vulkan_layer_abstraction_factory() = default;

void vulkan_layer_abstraction_factory::init_instance_internal(
    const renderer_properties &properties) {
  create_renderer(properties);
}
void vulkan_layer_abstraction_factory::create_renderer(
    const renderer_properties &properties) {
  auto create_fn_it = renderer_creation_fns.find(properties.m_renderer);
  AssertReturnIf(create_fn_it == renderer_creation_fns.end());

  m_renderer = create_fn_it->second(properties);
}

renderer_api &vulkan_layer_abstraction_factory::get_renderer_api() {
  return *m_renderer;
}

}  // namespace wunder