#include "scene/scene_manager.h"

#include "assets/asset_manager.h"
#include "assets/scene_asset.h"
#include "core/project.h"
#include "event/asset_events.h"
#include "event/event_handler.hpp"
#include "event/event_controller.h"
#include "event/scene_events.h"
#include "gla/vulkan/vulkan_scene.h"

namespace wunder {
scene_id scene_manager::s_scene_counter = 0;

scene_manager::scene_manager() : event_handler<asset_loaded>() {}

scene_manager::~scene_manager() /*override*/ = default;

bool scene_manager::activate_scene(scene_id id) {
  auto found_active_scene_it = m_active_scenes.find(id);
  ReturnIf(found_active_scene_it != m_active_scenes.end(), false);

  auto found_scene_asset_it = m_loaded_scenes.find(id);
  AssertReturnIf(found_scene_asset_it == m_loaded_scenes.end(), false);

  auto api_scene = vulkan_scene();
  api_scene.load_scene(found_scene_asset_it->second);

  m_active_scenes.emplace_back(std::make_pair(id, std::move(api_scene)));

  return true;
}

bool scene_manager::deactivate_scene(scene_id id) {
  auto active_scene_it = m_active_scenes.find(id);
  ReturnIf(active_scene_it == m_active_scenes.end(), false);
  m_active_scenes.erase(active_scene_it);
}

void scene_manager::on_event(
    const asset_loaded& asset_loaded_event) /*override*/
{
  auto& asset_manager = project::instance().get_asset_manager();
  auto maybe_scene_asset =
      asset_manager.find_asset<scene_asset>(asset_loaded_event.m_asset_handle);
  ReturnUnless(maybe_scene_asset.has_value());

  auto scene_id = s_scene_counter++;

  m_loaded_scenes.emplace_back(
      std::make_pair(scene_id, maybe_scene_asset.value().get()));

  event_controller::on_event<scene_loaded>({scene_id});
}

}  // namespace wunder