#ifndef GLTF_NORMALS_BUILDER_H
#define GLTF_NORMALS_BUILDER_H
#include <glm/vec3.hpp>
#include <vector>

namespace tinygltf {
class Model;
struct Primitive;
}  // namespace tinygltf
namespace wunder {
struct mesh_asset;
class mesh_asset_normals_builder {
 public:
  mesh_asset_normals_builder(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Primitive& gltf_primitive,
                       mesh_asset& out_mesh_asset);

 public:
  void build();

 private:
  void create_normals(std::vector<glm::vec3>& out_normals);

 private:
  const tinygltf::Model& m_gltf_scene_root;
  const tinygltf::Primitive& m_gltf_primitive;
  mesh_asset& m_out_mesh_asset;
};
}  // namespace wunder
#endif  // GLTF_NORMALS_BUILDER_H
