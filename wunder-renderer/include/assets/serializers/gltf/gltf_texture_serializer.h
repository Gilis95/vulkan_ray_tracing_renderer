#ifndef WUNDER_GLTF_TEXTURE_SERIALIZER_H
#define WUNDER_GLTF_TEXTURE_SERIALIZER_H

#include <vector>
#include <optional>

namespace tinygltf {
struct Texture;
class Model;
}

namespace wunder {
struct texture_asset;
class gltf_texture_serializer final {
 private:
  gltf_texture_serializer() = default;
 public:
  [[nodiscard]] static std::optional<texture_asset> process_texture(
      const tinygltf::Model& gltf_scene_root, const tinygltf::Texture& gltf_texture);
};
}  // namespace wunder
#endif  // WUNDER_GLTF_TEXTURE_SERIALIZER_H
