#ifndef GLTF_TANGENTS_BUILDER_H
#define GLTF_TANGENTS_BUILDER_H
#include <glm/fwd.hpp>
#include <vector>

namespace tinygltf {
class Model;
struct Primitive;
}  // namespace tinygltf

namespace wunder {
struct mesh_asset;
class mesh_asset_tangents_builder {
 public:
  mesh_asset_tangents_builder(const tinygltf::Model& gltf_scene_root,
                        const tinygltf::Primitive& gltf_primitive,
                        mesh_asset& out_mesh_asset);

 public:
  void build();

 private:
  void create_tangents(std::vector<glm::vec4>& out_tangents);

  // Given only a normal vector, finds a valid tangent.
  //
  // This uses the technique from "Improved accuracy when building an
  // orthonormal basis" by Nelson Max, https://jcgt.org/published/0006/01/02.
  // Any tangent-generating algorithm must produce at least one discontinuity
  // when operating on a sphere (due to the hairy ball theorem); this has a
  // small ring-shaped discontinuity at normal.z == -0.99998796.
  glm::vec4 make_fast_tangent(const glm::vec3& n);

 private:
  const tinygltf::Model& m_gltf_scene_root;
  const tinygltf::Primitive& m_gltf_primitive;
  mesh_asset& m_out_mesh_asset;
};
}  // namespace wunder
#endif  // GLTF_TANGENTS_BUILDER_H
