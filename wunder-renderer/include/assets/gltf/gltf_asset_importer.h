#ifndef WUNDER_GLTF_ASSET_IMPORTER_H
#define WUNDER_GLTF_ASSET_IMPORTER_H

#include <cstdint>
#include <expected>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// TODO:: This will include all possible components

#define KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME "KHR_lights_punctual"

namespace tinygltf {
class Model;
}

namespace wunder {
class scene_asset;
enum class asset_serialization_result_codes;
}  // namespace wunder

namespace wunder {
class asset_storage;
class gltf_asset_importer final {
 private:
  gltf_asset_importer() = default;
 public:
  static asset_serialization_result_codes import_asset(tinygltf::Model& gltf_model,
                                                    asset_storage& out_storage);
};
}  // namespace wunder
#endif  // WUNDER_GLTF_ASSET_IMPORTER_H