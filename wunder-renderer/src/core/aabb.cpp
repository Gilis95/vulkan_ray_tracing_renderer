#include "core/aabb.h"

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <vector>

namespace wunder {
aabb::aabb(const std::vector<glm::vec3>& corners)
  : m_min(std::numeric_limits<float>::max())
, m_max(std::numeric_limits<float>::lowest())
{
  for (auto& c : corners) {
    insert(c);
  }
}

void aabb::insert(const glm::vec3& vertex) {
  m_min = {std::min(m_min.x, vertex.x), std::min(m_min.y, vertex.y),
           std::min(m_min.z, vertex.z)};
  m_max = {std::max(m_max.x, vertex.x), std::max(m_max.y, vertex.y),
           std::max(m_max.z, vertex.z)};
}

void aabb::insert(const aabb& box) {
  insert(box.m_min);
  insert(box.m_max);
}

aabb aabb::transform(glm::mat4 mat) const {
  std::vector<glm::vec3> corners(8);
  corners[0] = glm::vec3(mat * glm::vec4(m_min.x, m_min.y, m_min.z, 1.f));
  corners[1] = glm::vec3(mat * glm::vec4(m_min.x, m_min.y, m_max.z, 1.f));
  corners[2] = glm::vec3(mat * glm::vec4(m_min.x, m_max.y, m_min.z, 1.f));
  corners[3] = glm::vec3(mat * glm::vec4(m_min.x, m_max.y, m_max.z, 1.f));
  corners[4] = glm::vec3(mat * glm::vec4(m_max.x, m_min.y, m_min.z, 1.f));
  corners[5] = glm::vec3(mat * glm::vec4(m_max.x, m_min.y, m_max.z, 1.f));
  corners[6] = glm::vec3(mat * glm::vec4(m_max.x, m_max.y, m_min.z, 1.f));
  corners[7] = glm::vec3(mat * glm::vec4(m_max.x, m_max.y, m_max.z, 1.f));

  aabb result(corners);
  return result;
}

}  // namespace wunder