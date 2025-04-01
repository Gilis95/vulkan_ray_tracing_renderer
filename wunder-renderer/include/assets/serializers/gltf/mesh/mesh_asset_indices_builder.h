#ifndef GLTF_INDICES_BUILDER_H
#define GLTF_INDICES_BUILDER_H

namespace tinygltf {
struct Primitive;
class Model;
}  // namespace tinygltf
namespace wunder {
struct mesh_asset;
class mesh_asset_indices_builder {
 public:
  mesh_asset_indices_builder(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Primitive& gltf_primitive,
                       mesh_asset& mesh_asset);
 public:
  bool build();

 private:
  const tinygltf::Model& m_gltf_scene_root;
  const tinygltf::Primitive& m_gltf_primitive;
  mesh_asset& m_mesh_asset;
};
}  // namespace wunder

#endif  // GLTF_INDICES_BUILDER_H
