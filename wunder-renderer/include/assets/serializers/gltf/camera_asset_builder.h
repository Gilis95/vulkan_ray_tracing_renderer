#ifndef WUNDER_GLTF_CAMERA_SERIALIZER_H
#define WUNDER_GLTF_CAMERA_SERIALIZER_H

#include <map>
#include <optional>
#include <string>

namespace tinygltf {
struct Camera;
class Value;
typedef std::map<std::string, Value> ExtensionMap;

}  // namespace tinygltf

namespace wunder {
struct camera_asset;
class camera_asset_builder final {
 public:
  camera_asset_builder(const tinygltf::Camera& gltf_camera,
                         const tinygltf::ExtensionMap& gltf_node_extensions);

 public:
  [[nodiscard]] std::optional<camera_asset> build();

 private:
  const tinygltf::Camera& gltf_camera;
  const tinygltf::ExtensionMap& gltf_node_extensions;
};
}  // namespace wunder
#endif  // WUNDER_GLTF_CAMERA_SERIALIZER_H
