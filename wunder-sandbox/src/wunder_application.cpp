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

#include "gla/vulkan/vulkan.h"
namespace wunder {

wunder_application::wunder_application(application_properties&& properties)
    : application(std::move(properties)),
      event_handler<wunder::event::scene_loaded>() {}

wunder_application::~wunder_application() /*override*/ = default;

/////////////////////////////////////////////////////////////////////////////////////////
void wunder_application::initialize_internal() {
  asset_manager& asset_manager = project::instance().get_asset_manager();
  // asset_manager.import_environment_map("wunder-renderer/resources/std_env.hdr");
  asset_manager.import_environment_map(
      "wunder-sandbox/resources/golden_gate.hdr");

  // asset_manager.import_asset("wunder-sandbox/resources/dragon/DragonAttenuation.gltf");
  // asset_manager.import_asset("wunder-sandbox/resources/sponza-gltf-pbr/Sponza.gltf");
  // asset_manager.import_asset("wunder-sandbox/resources/helmet/DamagedHelmet.gltf");
  // asset_manager.import_asset("wunder-sandbox/resources/box/glTF/Box.gltf");
  // asset_manager.import_asset("wunder-sandbox/resources/E39M5/scene.gltf");
  // asset_manager.import_asset("wunder-sandbox/resources/test/SpecularTest.gltf");
  // asset_manager.import_asset("wunder-sandbox/resources/test/untitled.gltf");
  initialize_imgui();
}

/////////////////////////////////////////////////////////////////////////////////////////
void wunder_application::shutdown_internal() /*override*/ {
  m_imgui.shutdown();
}

/////////////////////////////////////////////////////////////////////////////////////////
void wunder_application::initialize_imgui() { m_imgui.init(); }

/////////////////////////////////////////////////////////////////////////////////////////
application* create_application() {
  auto current_path = std::filesystem::current_path();
  wunder::wunder_filesystem::instance().set_work_dir(
      (std::filesystem::absolute(current_path)));

  uint32_t width =
      1920 + static_cast<std::uint32_t>(right_side_panel::s_dimensions.x);
  auto app_properties = application_properties{
      "wunder", "123",
      window_properties{
          "Wunder Application",
          width,
          1080, window_type::glfw},
      renderer_properties{.m_width = 1920,
                          .m_height = 1080,
                          .m_driver = driver::Vulkan,
                          .m_renderer = renderer_type::RAY_TRACE,
                          .m_gpu_to_use = gpu_to_use::Dedicated,
                          .m_enable_validation = true}};

  return new wunder_application(std::move(app_properties));
}

/////////////////////////////////////////////////////////////////////////////////////////
void wunder_application::update_internal(
    const time_unit& time_unit) /*override*/ {
  m_imgui.update(time_unit);
}

/////////////////////////////////////////////////////////////////////////////////////////
void wunder_application::on_event(
    const wunder::event::scene_loaded& scene_loaded_event) /*override*/
{
  project::instance().get_scene_manager().activate_scene(
      scene_loaded_event.m_id);
}
}  // namespace wunder
