#ifndef WUNDER_GLTF_SCENE_SERIALIZER_H
#define WUNDER_GLTF_SCENE_SERIALIZER_H

#include <cstdint>
#include <expected>
#include <string>
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

namespace wunder::gltf_scene_serializer {

std::expected<scene_asset, asset_serialization_result_codes> serialize(
    tinygltf::Model& gltf_scene_root);
};      // namespace wunder::gltf_scene_serializer
#endif  // WUNDER_GLTF_SCENE_SERIALIZER_H