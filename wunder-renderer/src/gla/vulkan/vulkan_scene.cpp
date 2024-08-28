#include "gla/vulkan/vulkan_scene.h"

#include "assets/scene_asset.h"
#include "core/wunder_macros.h"

namespace wunder {
void vulkan_scene::load_scene(scene_asset& asset) {
  auto mesh_entities =
      asset.filter_nodes<mesh_component, transform_component>();
  AssertReturnIf(mesh_entities.empty(), );  // nothing to render

  auto camera_entities = asset.filter_nodes<camera_component>();
  AssertReturnIf(camera_entities.empty(), );  // TODO:: Add default camera

  auto texture_entities = asset.filter_nodes<texture_component>();
  for (auto& texture_entity : texture_entities) {
    // TODO:: create texture buffer
  }

  auto material_entities = asset.filter_nodes<material_component>();
  for (auto& material_entity : material_entities) {
    // TODO:: create material buffer
  }

  for (auto mesh_entity : mesh_entities) {
    // TODO:: Create vertex and index buffers
  }
  // TODO:: Create acceleration structure

  auto& main_camera = camera_entities[0];  // TODO:: handle multiple cameras
}

}  // namespace wunder