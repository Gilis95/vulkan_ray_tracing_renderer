#ifndef WUNDER_GLTF_TEXTURE_SERIALIZER_H
#define WUNDER_GLTF_TEXTURE_SERIALIZER_H

#include <optional>
#include <vector>

namespace tinygltf {
struct Texture;
class Model;
}  // namespace tinygltf

namespace wunder {
struct texture_asset;
class texture_asset_builder final {
 public:
  texture_asset_builder(const tinygltf::Model& gltf_scene_root,
                       const tinygltf::Texture& gltf_texture);

 public:
  [[nodiscard]] std::optional<texture_asset> build();

 private:
  const tinygltf::Model& m_gltf_scene_root;
  const tinygltf::Texture& m_gltf_texture;
};
}  // namespace wunder
#endif  // WUNDER_GLTF_TEXTURE_SERIALIZER_H
