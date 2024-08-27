//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_AABB_H
#define WUNDER_AABB_H
#include "glm/vec3.hpp"
namespace wunder{
struct aabb
{
  glm::vec3 min, max;

  aabb()
      : min(0.0f), max(0.0f) {}

  aabb(const glm::vec3& min, const glm::vec3& max)
      : min(min), max(max) {}

  glm::vec3 size()  const { return max - min; }
  glm::vec3 center() const { return min + size() * 0.5f; }

};

}
#endif  // WUNDER_AABB_H
