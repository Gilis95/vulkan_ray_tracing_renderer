#ifndef GLTF_COLOUR_BUILDER_H
#define GLTF_COLOUR_BUILDER_H
#include <glm/fwd.hpp>
#include <vector>
namespace tinygltf {
struct Primitive;
class Model;
}  // namespace tinygltf

namespace wunder {
struct mesh_asset;

class mesh_asset_colour_builder {
 public:
  mesh_asset_colour_builder(const tinygltf::Model& gltf_scene_root,
                      const tinygltf::Primitive& gltf_primitive,
                      mesh_asset& mesh_asset);

 public:
  bool build();
private:
  std::vector<glm::vec4> create_mesh_colour() const;

 private:
  const tinygltf::Model& m_gltf_scene_root;
  const tinygltf::Primitive& m_gltf_primitive;
  mesh_asset& m_out_mesh_asset;
};
}  // namespace wunder
#endif  // GLTF_COLOUR_BUILDER_H
