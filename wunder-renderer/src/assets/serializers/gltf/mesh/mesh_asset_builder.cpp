#include "assets/asset_storage.h"
#include "assets/asset_types.h"
#include "assets/mesh_asset.h"
#include "assets/serializers/gltf/mesh/mesh_asset_builder.h"
#include "assets/serializers/gltf/mesh/mesh_asset_colour_builder.h"
#include "assets/serializers/gltf/mesh/mesh_asset_indices_builder.h"
#include "assets/serializers/gltf/mesh/mesh_asset_normals_builder.h"
#include "assets/serializers/gltf/mesh/mesh_asset_positions_builder.h"
#include "assets/serializers/gltf/mesh/mesh_asset_tangents_builder.h"
#include "assets/serializers/gltf/mesh/mesh_asset_uvs_builder.h"
#include "glm/vec4.hpp"
#include "tiny_gltf.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {

mesh_asset_builder::mesh_asset_builder(
    const tinygltf::Model& gltf_scene_root,
    const tinygltf::Primitive& gltf_primitive, const std::string& mesh_name,
    const std::unordered_map<uint32_t, asset_handle>& material_map)
    : gltf_scene_root(gltf_scene_root),
      gltf_primitive(gltf_primitive),
      mesh_name(mesh_name),
      material_map(material_map) {}

std::optional<mesh_asset> mesh_asset_builder::build() const {
  // Only triangles are supported
  // 0:point, 1:lines, 2:line_loop, 3:line_strip, 4:triangles, 5:triangle_strip,
  // 6:triangle_fan
  ReturnUnless(gltf_primitive.mode == 4, std::nullopt);

  mesh_asset mesh_asset;
  auto found_material_it = material_map.find(gltf_primitive.material);
  mesh_asset.m_material_handle = found_material_it == material_map.end()
                                     ? asset_handle::invalid()
                                     : found_material_it->second;

  mesh_asset_indices_builder indices_builder(gltf_scene_root, gltf_primitive,
                                       mesh_asset);
  mesh_asset_positions_builder positions_builder(gltf_scene_root, gltf_primitive,
                                           mesh_asset);
  mesh_asset_normals_builder normals_builder(gltf_scene_root, gltf_primitive,
                                       mesh_asset);
  mesh_asset_tangents_builder tangents_builder(gltf_scene_root, gltf_primitive,
                                         mesh_asset);
  mesh_asset_uvs_builder uvs_builder(gltf_scene_root, gltf_primitive, mesh_asset);
  mesh_asset_colour_builder colours_builder(gltf_scene_root, gltf_primitive,
                                      mesh_asset);

  AssertReturnUnless(indices_builder.build(), std::nullopt);
  AssertReturnUnless(positions_builder.build(), std::nullopt);
  normals_builder.build();
  tangents_builder.build();
  uvs_builder.build();
  colours_builder.build();

  return mesh_asset;
}


}  // namespace wunder
