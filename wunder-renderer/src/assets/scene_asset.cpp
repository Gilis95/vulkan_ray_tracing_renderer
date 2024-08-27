#include "assets/scene_asset.h"

#include "core/wunder_macros.h"
namespace wunder {
scene_node_id scene_asset::add_node(scene_node&& scene_node) {
  m_scene_nodes.emplace_back(scene_node);

  return m_scene_nodes.size() - 1;
}

void scene_asset::remove_node(scene_node_id id) {
  ReturnIf(m_scene_nodes.size() <= id);

  auto element_to_delete = m_scene_nodes.begin();
  std::advance(element_to_delete, id);
  m_scene_nodes.erase(element_to_delete);
}
}  // namespace wunder