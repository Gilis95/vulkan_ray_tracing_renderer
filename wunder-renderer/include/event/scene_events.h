#ifndef WUNDER_SCENE_EVENTS_H
#define WUNDER_SCENE_EVENTS_H

#include "scene/scene_types.h"

namespace wunder {
struct base_scene_event {
  scene_id m_id;
};
struct scene_loaded : public base_scene_event {};

struct scene_activated : base_scene_event {};
}  // namespace wunder
#endif  // WUNDER_SCENE_EVENTS_H
