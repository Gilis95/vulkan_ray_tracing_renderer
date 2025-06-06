#include "application.h"

#include "application_properties.h"
#include "assets/scene_asset.h"
#include "core/project.h"
#include "core/services_factory.h"
#include "core/wunder_macros.h"
#include "event/event_handler.hpp"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "include/gla/vulkan/ray-trace/vulkan_rtx_renderer.h"
#include "include/gla/vulkan/scene/vulkan_scene.h"
#include "scene/scene_manager.h"
#include "window/window_factory.h"

namespace wunder {
/////////////////////////////////////////////////////////////////////////////////////////
application::application(application_properties &&properties)
    : event_handler<wunder::event::window_close_event>(),
      m_is_running(false),
      m_properties(
          std::make_unique<application_properties>(std::move(properties))) {}

/////////////////////////////////////////////////////////////////////////////////////////
application::~application() = default;

/////////////////////////////////////////////////////////////////////////////////////////
void application::shutdown() {

  auto& graphic_abstraction_factory =
      vulkan::layer_abstraction_factory::instance();

  graphic_abstraction_factory.get_vulkan_context().mutable_swap_chain().shutdown();
  graphic_abstraction_factory.get_renderers().shutdown();

  project::instance().shutdown();
  service_factory::instance().shutdown();
  window_factory::instance().shutdown();
  graphic_abstraction_factory.shutdown();
  m_properties.reset();
}

/////////////////////////////////////////////////////////////////////////////////////////
void application::initialize() {
  log::init();

  AssertReturnUnless(
      window_factory::instance().initialize(m_properties->m_window_properties));
  vulkan::layer_abstraction_factory::instance().initialize(
      m_properties->m_renderer_properties);
  project::instance().initialize();
  service_factory::instance().initialize();
  initialize_internal();
}

/////////////////////////////////////////////////////////////////////////////////////////
void application::close() { m_is_running = false; }

/////////////////////////////////////////////////////////////////////////////////////////
void application::run() {
  m_is_running = true;
  auto &window = window_factory::instance().get_window();
  auto &gla = vulkan::layer_abstraction_factory::instance();
  auto &renderer = gla.get_renderers();
  auto &project = project::instance();

  time_unit frame_end = time_unit::from_current_time_in_miliseconds();
  while (m_is_running) {
    // WUNDER_WARN("Frame {0}", frame_number++);

    time_unit frame_start = time_unit::from_current_time_in_miliseconds();
    time_unit frame_duration = frame_start - frame_end;

    window.update(frame_duration);

    renderer.update(frame_duration);

    project.update(frame_duration);

    frame_end = frame_start;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void application::on_event(
    const wunder::event::window_close_event &) /*override*/
{
  m_is_running = false;
}

}  // namespace wunder
