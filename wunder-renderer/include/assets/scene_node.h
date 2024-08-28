#ifndef WUNDER_SCENE_NODE_H
#define WUNDER_SCENE_NODE_H

#include <variant>
#include <vector>

#include "assets/components/camera_component.h"
#include "assets/components/light_component.h"
#include "assets/components/material_component.h"
#include "assets/components/mesh_component.h"
#include "assets/components/texture_component.h"
#include "assets/components/transform_component.h"
#include "entity/entity.h"

namespace wunder {
using scene_node =
    entity<camera_component, material_component, mesh_component,
           light_component, texture_component, transform_component>;
}  // namespace wunder
#endif  // WUNDER_SCENE_NODE_H
