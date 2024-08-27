//
// Created by christian on 8/27/24.
//

#ifndef WUNDER_GLTF_SCENE_NODE_CREATOR_H
#define WUNDER_GLTF_SCENE_NODE_CREATOR_H

#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "glm/fwd.hpp"

namespace tinygltf {
struct Light;
struct Value;
}  // namespace tinygltf

namespace wunder {
class scene_asset;
class mesh_component;
struct transform_component;

class gltf_scene_node_creator {
 public:
  gltf_scene_node_creator(glm::mat4& model_matrix, scene_asset& scene_asset);

 public:
  virtual void create() = 0;

  transform_component get_transform_component() const;

 private:
  glm::mat4& m_model_matrix;

 protected:
  scene_asset& m_scene_asset;
};

class gltf_mesh_node_creator : gltf_scene_node_creator {
 public:
  gltf_mesh_node_creator(glm::mat4& m_model_matrix, scene_asset& m_scene_asset,
                         const std::vector<mesh_component>& mesh_components);

 public:
  void create() override;

 private:
  const std::vector<mesh_component>& m_mesh_components;
};

class gltf_camera_node_creator : gltf_scene_node_creator {
 public:
  gltf_camera_node_creator(glm::mat4& m_model_matrix,
                           scene_asset& m_scene_asset);

 public:
  void create() override;

 private:
  std::map<std::string, tinygltf::Value> m_gltf_node_extensions_map;
};

class gltf_light_node_creator : gltf_scene_node_creator {
 public:
  gltf_light_node_creator(glm::mat4& model_matrix, scene_asset& scene_asset,
                          const tinygltf::Light& gltf_light);

 public:
  void create() override;

 private:
  const tinygltf::Light& m_gltf_light;
};
}  // namespace wunder
#endif  // WUNDER_GLTF_SCENE_NODE_CREATOR_H
