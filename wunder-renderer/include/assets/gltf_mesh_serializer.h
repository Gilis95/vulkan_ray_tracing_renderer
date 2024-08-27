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

namespace tinygltf {
class Model;
struct Primitive;
}

namespace wunder {
struct mesh_component;

class gltf_mesh_serializer {
 public:
  // one mesh consists of multiple primitives, because different part of the
  // mesh could reference different materials. However the easiest way for us
  // to represent such beahviour in ray tracing context is creating separate
  // meshes, so we can use the index of hit mesh to lookup for material. This
  // on the other hand leads to one more complication, mesh transformation is
  // parsed in the next stage and should be applied to all primitives of
  // single mesh
  static std::unordered_map<std::uint32_t /*mesh_id*/,
                            std::vector<mesh_component> /*mesh_primitives*/>
  process_meshes(tinygltf::Model& gltf_scene_root,
                 const std::unordered_set<uint32_t>& mesh_indices);

 private:
  static std::optional<mesh_component> process_mesh(
      const tinygltf::Model& gltf_scene_root,
      const tinygltf::Primitive& gltf_primitive, const std::string& mesh_name);

  static bool try_parse_indices(const tinygltf::Model& gltf_scene_root,
                                const tinygltf::Primitive& gltf_primitive,
                                mesh_component& mesh_component);

  static bool try_parse_positions(const tinygltf::Model& gltf_scene_root,
                                  const tinygltf::Primitive& gltf_primitive,
                                  mesh_component& mesh_component);
  static void try_parse_mesh_tangents(const tinygltf::Model& gltf_scene_root,
                                      const tinygltf::Primitive& gltf_primitive,
                                      mesh_component& mesh_component);
  static void try_parse_mesh_colour(const tinygltf::Model& gltf_scene_root,
                                    const tinygltf::Primitive& gltf_primitive,
                                    mesh_component& mesh_component);
  static void try_parse_uvs(const tinygltf::Model& gltf_scene_root,
                            const tinygltf::Primitive& gltf_primitive,
                            mesh_component& out_mesh_component);
  static void try_parse_normals(const tinygltf::Model& gltf_scene_root,
                                const tinygltf::Primitive& gltf_primitive,
                                mesh_component& out_mesh_component);
  static void try_parse_aabb(const tinygltf::Model& gltf_scene_root,
                             const tinygltf::Primitive& gltf_primitive,
                             mesh_component& mesh_component);
};
}  // namespace wunder
#endif  // WUNDER_GLTF_MESH_SERIALIZER_H
