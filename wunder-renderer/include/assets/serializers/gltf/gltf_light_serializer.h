
#ifndef WUNDER_GLTF_LIGHT_SERIALIZER_H
#define WUNDER_GLTF_LIGHT_SERIALIZER_H

#include <optional>

namespace tinygltf {
struct Light;
}
namespace wunder {
struct light_asset;
class gltf_light_serializer final {
 private:
  gltf_light_serializer() = default;

 public:
  [[nodiscard]] static std::optional<light_asset> serialize(
      const tinygltf::Light& gltf_light);
};
}  // namespace wunder
#endif  // WUNDER_GLTF_LIGHT_SERIALIZER_H
