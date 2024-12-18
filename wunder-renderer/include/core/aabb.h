//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_AABB_H
#define WUNDER_AABB_H
#include "glm/vec3.hpp"
namespace wunder{
struct aabb
{
 public:
  aabb()
      : m_min(0.0f), m_max(0.0f) {}

  aabb(const glm::vec3& min, const glm::vec3& max)
      : m_min(min), m_max(max) {}
 public:
  [[nodiscard]] glm::vec3 size()  const { return m_max - m_min; }
  [[nodiscard]] glm::vec3 center() const { return m_min + size() * 0.5f; }

  void insert(const glm::vec3& vertex);
  void insert(const aabb& box);
 public:
  glm::vec3 m_min, m_max;
};

}
#endif  // WUNDER_AABB_H
