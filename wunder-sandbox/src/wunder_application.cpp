#include "wunder_application.h"

#include <application_properties.h>
#include <assets/asset_manager.h>
#include <core/project.h>
#include <core/wunder_filesystem.h>
#include <entry_point.h>
#include <event/asset_events.h>
#include <event/scene_events.h>
#include <gla/renderer_properties.h>
#include <scene/scene_manager.h>
#include <window/window_properties.h>

#include <event/event_handler.hpp>
#include <utility>

namespace wunder {

wunder_application::wunder_application(application_properties&& properties)
    : application(std::move(properties)),
      event_handler<wunder::event::scene_loaded>() {}

wunder_application::~wunder_application() /*override*/ = default;

void wunder_application::initialize_internal() {
  asset_manager& asset_manager = project::instance().get_asset_manager();
  asset_manager.import_environment_map("wunder-renderer/resources/std_env.hdr");
  asset_manager.import_asset("wunder-sandbox/resources/cube.gltf");
}

/////////////////////////////////////////////////////////////////////////////////////////
application* create_application() {
  auto current_path = std::filesystem::current_path();
  wunder::wunder_filesystem::instance().set_work_dir(
      (std::filesystem::absolute(current_path / ".." / "..")));

  auto app_properties = application_properties{
      "wunder", "123",
      window_properties{"Wunder Application", 1500, 750, window_type::glfw},
      renderer_properties{.m_width = 1500,
                          .m_height = 750,
                          .m_driver = driver::Vulkan,
                          .m_renderer = renderer_type::RAY_TRACE,
                          .m_gpu_to_use = gpu_to_use::Dedicated,
                          .m_enable_validation = false}};

  return new wunder_application(std::move(app_properties));
}

/////////////////////////////////////////////////////////////////////////////////////////
void wunder_application::on_event(
    const wunder::event::scene_loaded& scene_loaded_event) /*override*/
{
  project::instance().get_scene_manager().activate_scene(
      scene_loaded_event.m_id);
}
}  // namespace wunder
