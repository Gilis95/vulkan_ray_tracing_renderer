#ifndef WUNDER_SCENE_MANAGER_H
#define WUNDER_SCENE_MANAGER_H

#include "core/vector_map.h"

#include "event/event_handler.h"
#include "scene/scene_types.h"

namespace tinygltf {
class Model;
}

namespace wunder {
struct asset_loaded;

class scene_asset;
class vulkan_scene;
enum class asset_serialization_result_codes;

class scene_manager : public event_handler<asset_loaded> {
 public:
  scene_manager();
  ~scene_manager() override;

  bool activate_scene(scene_id id);
  bool deactivate_scene(scene_id id);

 protected:
  void on_event(const asset_loaded&) override;

 private:
  vector_map<scene_id, scene_asset> m_loaded_scenes;
  vector_map<scene_id, vulkan_scene> m_active_scenes;

  static scene_id s_scene_counter;
};
}  // namespace wunder
#endif  // WUNDER_SCENE_MANAGER_H