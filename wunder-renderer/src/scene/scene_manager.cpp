#include "scene/scene_manager.h"

#include "assets/asset_manager.h"
#include "assets/scene_asset.h"
#include "core/project.h"
#include "event/asset_events.h"
#include "event/event_controller.h"
#include "event/event_handler.hpp"
#include "event/scene_events.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure.h"
#include "gla/vulkan/scene/vulkan_mesh_scene_node.h"
#include "gla/vulkan/scene/vulkan_scene.h"

namespace wunder {
scene_id scene_manager::s_scene_counter = 0;

scene_manager::scene_manager() : event_handler<asset_loaded>() {}

scene_manager::~scene_manager() /*override*/ = default;

optional_ref<vulkan::scene> scene_manager::mutable_api_scene(scene_id id) {
  static optional_ref<vulkan::scene> s_empty = std::nullopt;

  auto found_active_scene_it = m_active_scenes.find(id);
  return found_active_scene_it == m_active_scenes.end()
             ? s_empty
             : found_active_scene_it->second;
}

optional_const_ref<scene_asset> scene_manager::get_scene_asset(
    scene_id id) const {
  static optional_ref<scene_asset> s_empty = std::nullopt;

  auto found_scene_asset_it = m_loaded_scenes.find(id);
  ReturnIf(found_scene_asset_it == m_loaded_scenes.end(), s_empty);

  return found_scene_asset_it->second;
}

bool scene_manager::activate_scene(scene_id id) {
  auto found_active_scene_it = m_active_scenes.find(id);
  ReturnIf(found_active_scene_it != m_active_scenes.end(), false);

  auto found_scene_asset_it = m_loaded_scenes.find(id);
  AssertReturnIf(found_scene_asset_it == m_loaded_scenes.end(), false);

  std::pair<scene_id, vulkan::scene> l;

  vulkan::scene api_scene;
  auto& [scene_id, scene] = m_active_scenes.emplace_back();
  scene.load_scene(found_scene_asset_it->second);
  scene_id = id;

  event_controller::on_event<wunder::event::scene_activated>({scene_id});

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

  event_controller::on_event<wunder::event::scene_loaded>({scene_id});
}

}  // namespace wunder