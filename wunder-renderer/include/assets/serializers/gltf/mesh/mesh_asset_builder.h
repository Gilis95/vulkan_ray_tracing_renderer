//
// Created by christian on 8/27/24.
//

#ifndef WUNDER_GLTF_MESH_SERIALIZER_H
#define WUNDER_GLTF_MESH_SERIALIZER_H

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "assets/asset_types.h"

namespace tinygltf {
class Model;
struct Primitive;
}  // namespace tinygltf

namespace wunder {
struct mesh_asset;

class mesh_asset_builder final {
 public:
  mesh_asset_builder(
      const tinygltf::Model& gltf_scene_root,
      const tinygltf::Primitive& gltf_primitive, const std::string& mesh_name,
      const std::unordered_map<uint32_t, asset_handle>& material_map);

 public:
  // one mesh consists of multiple primitives, because different part of the
  // mesh could reference different materials. However the easiest way for us
  // to represent such beahviour in ray tracing context is creating separate
  // meshes, so we can use the index of hit mesh to lookup for material. This
  // on the other hand leads to one more complication, mesh transformation is
  // parsed in the next stage and should be applied to all primitives of
  // single mesh
  [[nodiscard]] std::optional<mesh_asset> build() const;

 private:
  void try_parse_aabb(const tinygltf::Model& gltf_scene_root,
                      const tinygltf::Primitive& gltf_primitive,
                      mesh_asset& mesh_asset);

 private:
  const tinygltf::Model& gltf_scene_root;
  const tinygltf::Primitive& gltf_primitive;
  const std::string& mesh_name;
  const std::unordered_map<uint32_t, asset_handle>& material_map;
};
}  // namespace wunder
#endif  // WUNDER_GLTF_MESH_SERIALIZER_H
