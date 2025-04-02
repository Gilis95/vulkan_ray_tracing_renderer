#include "scene/scene_load_task.h"

#include "assets/scene_asset.h"
#include "event/event_controller.h"
#include "event/scene_events.h"
#include "gla/vulkan/scene/vulkan_scene.h"

namespace wunder {
scene_load_task::scene_load_task(scene_id id, vulkan::scene& scene,
                                 const scene_asset& input_scene_asset)
    : m_id(id), m_out_scene(scene), m_input_scene_asset(input_scene_asset) {}

void scene_load_task::run() /*override*/ {
  m_out_scene.load_scene(m_input_scene_asset);
}

void scene_load_task::execute_on_main_thread() /*override*/ {
  event_controller::on_event<wunder::event::scene_activated>({m_id});
}
}  // namespace wunder