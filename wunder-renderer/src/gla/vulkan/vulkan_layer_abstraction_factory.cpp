#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

#include <functional>
#include <initializer_list>
#include <memory>

#include "core/vector_map.h"
#include "gla/renderer_capabilities .h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/ray-trace/vulkan_rtx_renderer.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "gla/vulkan/vulkan_shader.h"

namespace {
std::unique_ptr<wunder::vulkan::rtx_renderer> create_ray_trace_vulkan_render(
    const wunder::renderer_properties &properties) {
  auto renderer = std::make_unique<wunder::vulkan::rtx_renderer>(properties);
  renderer->init(properties);

  return renderer;
}

using renderer_create_fn =
    std::function<std::unique_ptr<wunder::vulkan::rtx_renderer>(
        const wunder::renderer_properties &)>;

auto ray_tracing_renderer_mapping =
    std::make_pair<wunder::renderer_type, renderer_create_fn>(
        wunder::renderer_type::RAY_TRACE,
        std::bind(create_ray_trace_vulkan_render, std::placeholders::_1));

wunder::vector_map<wunder::renderer_type, renderer_create_fn>
    renderer_creation_fns({ray_tracing_renderer_mapping});
}  // namespace

namespace wunder::vulkan {
layer_abstraction_factory::layer_abstraction_factory() = default;

layer_abstraction_factory::~layer_abstraction_factory() = default;

layer_abstraction_factory &layer_abstraction_factory::instance() {
  static layer_abstraction_factory s_instance;
  return s_instance;
}

void layer_abstraction_factory::initialize(
    const renderer_properties &properties) {
  create_vulkan_context(properties);
  create_renderer(properties);
}

void layer_abstraction_factory::create_renderer(
    const renderer_properties &properties) {
  auto create_fn_it = renderer_creation_fns.find(properties.m_renderer);
  AssertReturnIf(create_fn_it == renderer_creation_fns.end());

  m_renderer = create_fn_it->second(properties);
}

void layer_abstraction_factory::create_vulkan_context(
    const renderer_properties &properties) {
  m_context = std::make_unique<context>();
  m_context->init(properties);
}

rtx_renderer &layer_abstraction_factory::get_renderers() { return *m_renderer; }

context &layer_abstraction_factory::get_vulkan_context() { return *m_context; }

void layer_abstraction_factory::shutdown() {
  if (m_renderer) {
    m_renderer->shutdown();
    m_renderer.reset();
  }

  if (m_context.get()) {
    m_context->shutdown();
    m_context.reset();
  }
}

}  // namespace wunder::vulkan