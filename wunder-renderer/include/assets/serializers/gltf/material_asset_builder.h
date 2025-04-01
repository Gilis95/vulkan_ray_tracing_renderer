//
// Created by christian on 8/28/24.
//

#ifndef WUNDER_GLTF_MATERIAL_SERIALIZER_H
#define WUNDER_GLTF_MATERIAL_SERIALIZER_H

#include <cstdint>
#include <optional>
#include <unordered_map>

#include "assets/asset_types.h"

namespace tinygltf {
struct Material;
}

namespace wunder {
struct material_asset;
class material_asset_builder final {
 public:
  material_asset_builder(
      const tinygltf::Material& gltf_material,
      const std::unordered_map<std::uint32_t, asset_handle>& textures_map);

 public:
  material_asset build();

 private:
  const tinygltf::Material& m_gltf_material;
  const std::unordered_map<std::uint32_t, asset_handle>& m_textures_map;
};
}  // namespace wunder
#endif  // WUNDER_GLTF_MATERIAL_SERIALIZER_H
