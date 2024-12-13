#include "application.h"

#include <tracy/Tracy.hpp>

#include "application_properties.h"
#include "assets/scene_asset.h"
#include "core/project.h"
#include "core/wunder_macros.h"
#include "event/event_handler.hpp"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_renderer.h"
#include "include/gla/vulkan/scene/vulkan_scene.h"
#include "scene/scene_manager.h"
#include "window/window_factory.h"

namespace wunder {
/////////////////////////////////////////////////////////////////////////////////////////
application::application(application_properties &&properties)
    : event_handler<window_close_event>(),
      m_is_running(false),
      m_properties(
          std::make_unique<application_properties>(std::move(properties))) {}

/////////////////////////////////////////////////////////////////////////////////////////
application::~application() {
  project::instance().shutdown();
  vulkan::layer_abstraction_factory::instance().shutdown();
  window_factory::instance().shutdown();
}

/////////////////////////////////////////////////////////////////////////////////////////
void application::initialize() {
  std::string application_revision("Revision: 1234");
  std::string application_name("application Name: wunder");

  log::init();

  TracyAppInfo(m_properties->m_debug_name.c_str(),
               m_properties->m_debug_name.size());
  TracyAppInfo(m_properties->m_debug_version.c_str(),
               m_properties->m_debug_version.size());

  AssertReturnUnless(
      window_factory::instance().initialize(m_properties->m_window_properties));
  vulkan::layer_abstraction_factory::instance().initialize(
      m_properties->m_renderer_properties);
  project::instance().initialize();

  initialize_internal();
}

/////////////////////////////////////////////////////////////////////////////////////////
void application::close() { m_is_running = false; }

/////////////////////////////////////////////////////////////////////////////////////////
void application::run() {
  m_is_running = true;
  auto &window = window_factory::instance().get_window();
  auto &gla = vulkan::layer_abstraction_factory::instance();
  auto &renderers = gla.get_renderers();

  while (m_is_running) {
    window.update(0);
    for (auto &[_, renderer] : renderers) {
      renderer->update(0);
    }
    FrameMark;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////
void application::on_event(const window_close_event &) /*override*/
{
  m_is_running = false;
}

}  // namespace wunder
