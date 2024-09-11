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
class gltf_camera_serializer final {
 private:
  gltf_camera_serializer() = default;

 public:
  [[nodiscard]] static std::optional<camera_asset> serialize(
      const tinygltf::Camera& gltf_camera,
      tinygltf::ExtensionMap gltf_node_extensions);
};
}  // namespace wunder
#endif  // WUNDER_GLTF_CAMERA_SERIALIZER_H
