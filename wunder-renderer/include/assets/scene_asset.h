
#ifndef WUNDER_SCENE_ASSET_H
#define WUNDER_SCENE_ASSET_H
#include <vector>
#include "assets/scene_node.h"

namespace wunder{

using scene_node_id = std::uint32_t;

class scene_asset{
 public:
  template <typename visitor_type>
  void iterate_nodes_components(visitor_type& visitor);

  scene_node_id add_node(scene_node&& scene_node);
  void remove_node(scene_node_id id);

 private:
  std::vector<scene_node> m_scene_nodes;
};

template <typename visitor_type>
void scene_asset::iterate_nodes_components(visitor_type& visitor)
{
  for(auto& scene_node : m_scene_nodes)
  {
    scene_node.iterate_components(visitor);
  }
}
}
#endif  // WUNDER_SCENE_ASSET_H
