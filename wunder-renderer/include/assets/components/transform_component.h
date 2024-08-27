#ifndef WUNDER_TRANSFORM_COMPONENT_H
#define WUNDER_TRANSFORM_COMPONENT_H

#include "glm/vec3.hpp"

namespace wunder {
struct transform_component {
  glm::vec3 m_translation;
  glm::vec3 m_scale;
  glm::vec3 m_rotation;
  glm::mat4 m_world_matrix;
};
}  // namespace wunder
#endif  // WUNDER_TRANSFORM_COMPONENT_H
