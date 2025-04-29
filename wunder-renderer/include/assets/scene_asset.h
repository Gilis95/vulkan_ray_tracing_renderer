
#ifndef WUNDER_SCENE_ASSET_H
#define WUNDER_SCENE_ASSET_H
#include <vector>

#include "assets/scene_node.h"
#include "core/aabb.h"

namespace wunder {

using scene_node_id = std::uint64_t;

class scene_asset {
 public:
  template <typename visitor_type>
  void iterate_nodes_components(visitor_type& visitor);

  scene_node_id add_node(scene_node&& scene_node);
  void remove_node(scene_node_id id);

  template <typename... types>
  [[nodiscard]] std::vector<std::reference_wrapper<scene_node>> filter_nodes();

  template <typename... types>
  [[nodiscard]] std::vector<const_ref<const scene_node>>
  filter_nodes() const;

  [[nodiscard]] aabb& mutable_aabb() { return m_aabb; }
  [[nodiscard]] const aabb& get_aabb() const { return m_aabb; }

 private:
  std::vector<scene_node> m_scene_nodes;
  aabb m_aabb;
};

template <typename visitor_type>
void scene_asset::iterate_nodes_components(visitor_type& visitor) {
  for (auto& scene_node : m_scene_nodes) {
    scene_node.iterate_components(visitor);
  }
}

template <typename... types>
std::vector<std::reference_wrapper<scene_node>> scene_asset::filter_nodes() {
  std::vector<std::reference_wrapper<scene_node>> result;

  for (auto& scene_node : m_scene_nodes) {
    if (scene_node.has_components<types...>()) {
      result.emplace_back(scene_node);
    }
  }

  return result;
}

template <typename... types>
std::vector<std::reference_wrapper<const scene_node>>
scene_asset::filter_nodes() const {
  std::vector<std::reference_wrapper<const scene_node>> result;

  for (const auto& scene_node : m_scene_nodes) {
    if (scene_node.has_components<types...>()) {
      result.emplace_back(scene_node);
    }
  }

  return result;
}
}  // namespace wunder
#endif  // WUNDER_SCENE_ASSET_H
