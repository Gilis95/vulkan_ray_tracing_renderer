#include "core/aabb.h"

#include <algorithm>

namespace wunder {
void aabb::insert(const glm::vec3& vertex) {
  m_min = {std::min(m_min.x, vertex.x), std::min(m_min.y, vertex.y),
           std::min(m_min.z, vertex.z)};
  m_max = {std::max(m_max.x, vertex.x), std::max(m_max.y, vertex.y),
           std::max(m_max.z, vertex.z)};
}

void aabb::insert(const aabb& box){
  insert(box.m_min);
  insert(box.m_max);
}

}  // namespace wunder