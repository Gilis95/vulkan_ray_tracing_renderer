
#ifndef WUNDER_GLTF_LIGHT_SERIALIZER_H
#define WUNDER_GLTF_LIGHT_SERIALIZER_H

#include <optional>

namespace tinygltf {
struct Light;
}
namespace wunder {
struct light_asset;
class light_asset_builder final {
 public:
  explicit light_asset_builder(const tinygltf::Light& gltf_light);

 public:
  [[nodiscard]] std::optional<light_asset> build();

 private:
  const tinygltf::Light& gltf_light;
};
}  // namespace wunder
#endif  // WUNDER_GLTF_LIGHT_SERIALIZER_H
