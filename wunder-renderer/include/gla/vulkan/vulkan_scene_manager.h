//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_VULKAN_SCENE_MANAGER_H
#define WUNDER_VULKAN_SCENE_MANAGER_H
#include "scene/scene_types.h"

namespace wunder {

struct camera_component;
struct material_component;
struct mesh_component;
struct transform_component;

class vulkan_scene_manager {
 public:
  void start_binding(scene_id id);
  void finish_binding(scene_id id);

  void unbind(scene_id id);

 public:
  void operator()(const camera_component& component);
  void operator()(const material_component& component);
  void operator()(const mesh_component&);
  void operator()(const transform_component& component);
  private
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_SCENE_MANAGER_H
