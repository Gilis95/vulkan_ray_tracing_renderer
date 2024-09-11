//
// Created by christian on 9/10/24.
//

#ifndef WUNDER_SCENE_COMPONENTS_H
#define WUNDER_SCENE_COMPONENTS_H

#include "assets/asset_types.h"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

namespace wunder {
struct base_asset_referencing_component {
  asset_handle m_handle = -1;
};

struct mesh_component : public base_asset_referencing_component {
};

struct material_component : public base_asset_referencing_component {
  };

struct camera_component: public base_asset_referencing_component {
};

struct texture_component: public base_asset_referencing_component {};
struct light_component: public base_asset_referencing_component {};


struct transform_component {
  glm::vec3 m_translation;
  glm::vec3 m_scale;
  glm::vec3 m_rotation;
  glm::mat4 m_world_matrix;
};

}  // namespace wunder
#endif  // WUNDER_SCENE_COMPONENTS_H
