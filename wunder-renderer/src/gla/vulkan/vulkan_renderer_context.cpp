#include "gla/vulkan/vulkan_renderer_context.h"

#include "application_properties.h"
#include "core/services_factory.h"
#include "core/time_unit.h"
#include "event/scene_events.h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/rasterize/vulkan_rasterize_renderer.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/ray-trace/vulkan_rtx_pipeline.h"
#include "gla/vulkan/ray-trace/vulkan_rtx_renderer.h"

namespace wunder::vulkan {
renderer_context::renderer_context(const application_properties& properties)
    : m_have_active_scene(false),
      m_renderer_properties(properties.m_renderer_properties),
      m_swap_chain(
          make_unique<swap_chain>(properties.m_window_properties.m_width, properties.m_window_properties.m_height)),
      m_rasterize_renderer(make_unique<rasterize_renderer>(properties.m_renderer_properties)),
      m_rtx_renderer(make_unique<rtx_renderer>(properties.m_renderer_properties)) {}

renderer_context::~renderer_context() /*override*/ = default;

const renderer_properties& renderer_context::get_renderer_properties() const {
  return m_renderer_properties;
}

swap_chain& renderer_context::mutable_swap_chain() { return *m_swap_chain; }

rasterize_renderer& renderer_context::mutable_rasterize_renderer() {
  return *m_rasterize_renderer;
}

rtx_renderer& renderer_context::mutable_rtx_renderer() {
  return *m_rtx_renderer;
}

void renderer_context::shutdown() {
  if (m_swap_chain.get()) {
    m_swap_chain->shutdown();
    m_swap_chain.reset();
  }

  if (m_rasterize_renderer.get()) {
    m_rasterize_renderer->shutdown();
    m_rasterize_renderer.reset();
  }

  if (m_rtx_renderer.get()) {
    m_rtx_renderer->shutdown();
    m_rtx_renderer.reset();
  }
}

void renderer_context::init() {
  m_swap_chain->init();
}

bool renderer_context::begin() {
  AssertReturnUnless(m_swap_chain->acquire().has_value(), false);

  m_swap_chain->begin_command_buffer();

  return true;
}

void renderer_context::update(time_unit dt) {
  ReturnUnless(m_have_active_scene);

  service_factory::instance().update(dt);

  m_rtx_renderer->update(dt);
  m_rasterize_renderer->update(dt);

}

void renderer_context::end() {
  m_swap_chain->flush_current_command_buffer();
}

void renderer_context::on_event(
    const wunder::event::scene_activated& event) /*override*/ {
  m_rasterize_renderer->init(event.m_id);
  m_rtx_renderer->init(event.m_id);

  log_loaded_scene_size();

  m_have_active_scene = true;
}

void renderer_context::log_loaded_scene_size() {
#if PRINT_ALLOCATED_SCENE_SIZE
  auto& memory_allocator = layer_abstraction_factory::instance()
                               .get_vulkan_context()
                               .mutable_resource_allocator();

  memory_allocator.dump_stats();
#endif
}

}  // namespace wunder::vulkan