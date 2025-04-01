#ifndef GLTF_UVS_BUILDER_H
#define GLTF_UVS_BUILDER_H
#include <glm/fwd.hpp>
#include <vector>

namespace tinygltf {
struct Primitive;
class Model;
}  // namespace tinygltf
namespace wunder {
struct mesh_asset;
class mesh_asset_uvs_builder {
 public:
  mesh_asset_uvs_builder(const tinygltf::Model& gltf_scene_root,
                   const tinygltf::Primitive& gltf_primitive,
                   mesh_asset& out_mesh_asset);

 public:
  void build();

 private:
  void create_texcoords(
                        std::vector<glm::vec2>& out_tex_coords) const;

 private:
  const tinygltf::Model& m_gltf_scene_root;
  const tinygltf::Primitive& m_gltf_primitive;
  mesh_asset& m_out_mesh_asset;
};
}  // namespace wunder
#endif  // GLTF_UVS_BUILDER_H
