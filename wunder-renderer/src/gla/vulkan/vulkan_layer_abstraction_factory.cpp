#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

#include <functional>
#include <initializer_list>
#include <memory>

#include "core/vector_map.h"
#include "gla/renderer_capabilities .h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "gla/vulkan/vulkan_pipeline.h"
#include "gla/vulkan/vulkan_renderer.h"
#include "gla/vulkan/vulkan_shader.h"
#include "gla/vulkan/vulkan_shader_binding_table.h"

namespace {
std::unique_ptr<wunder::vulkan::renderer> create_ray_trace_vulkan_render(
    const wunder::renderer_properties &properties) {
  auto renderer = std::make_unique<wunder::vulkan::renderer>();
  renderer->init(properties);

  return renderer;
}

using renderer_create_fn =
    std::function<std::unique_ptr<wunder::vulkan::renderer>(
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

  m_renderers.emplace_back(create_fn_it->first, create_fn_it->second(properties));
}

void layer_abstraction_factory::create_vulkan_context(
    const renderer_properties &properties) {
  m_context = std::make_unique<context>();
  m_context->init(properties);
}

optional_ref<renderer> layer_abstraction_factory::get_renderer_api(renderer_type type) {
  static optional_ref<renderer> s_empty = std::nullopt;

  auto found_renderer_it = m_renderers.find(type);
  return found_renderer_it == m_renderers.end() ? s_empty
                                                : *found_renderer_it->second;
}

vector_map<renderer_type, unique_ptr<renderer>> &
layer_abstraction_factory::get_renderers() {
  return m_renderers;
}

context &layer_abstraction_factory::get_vulkan_context() {
  return *m_context;
}

void layer_abstraction_factory::shutdown() {
  for (auto &[identifier, renderer] : m_renderers) {
    AssertLogUnless(renderer.release());
  }

  if (m_context.get()) {
    AssertLogUnless(m_context.release());
  }
}

}  // namespace wunder