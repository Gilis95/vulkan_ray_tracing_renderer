//
// Created by christian on 8/28/24.
//

#ifndef WUNDER_GLTF_MATERIAL_SERIALIZER_H
#define WUNDER_GLTF_MATERIAL_SERIALIZER_H

#include <unordered_map>
#include <cstdint>
#include <optional>

#include "assets/asset_types.h"

namespace tinygltf {
struct Material;
}

namespace wunder {
struct material_asset;
class gltf_material_importer final{
 private:
  gltf_material_importer() = default;
 public:
  static material_asset process_material(
      const tinygltf::Material& gltf_material,
      const std::unordered_map<std::uint32_t, asset_handle>& textures_map);
};
}  // namespace wunder
#endif  // WUNDER_GLTF_MATERIAL_SERIALIZER_H
