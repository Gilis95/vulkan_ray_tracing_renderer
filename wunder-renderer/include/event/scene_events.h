#ifndef WUNDER_SCENE_EVENTS_H
#define WUNDER_SCENE_EVENTS_H

#include "scene/scene_types.h"

namespace wunder::event {
struct base_scene {
  scene_id m_id;
};
struct scene_loaded : public base_scene {};

struct scene_activated : base_scene {};
}  // namespace wunder
#endif  // WUNDER_SCENE_EVENTS_H
