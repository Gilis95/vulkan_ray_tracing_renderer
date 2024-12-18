#include "assets/serializers/gltf/gltf_light_serializer.h"

#include "include/assets/light_asset.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {
std::optional<light_asset> gltf_light_serializer::serialize(
    const tinygltf::Light& gltf_light) {
  auto maybe_colour =
      tinygltf::utils::vector_to_glm<glm::vec4>(gltf_light.color);
  AssertReturnUnless(maybe_colour.has_value(), std::nullopt);
  speecific_light_type_data light_specific_data;
  if (gltf_light.type == "point") {
    light_specific_data = point_light{};
  } else if (gltf_light.type == "directional") {
    light_specific_data = directional_light{};
  } else if (gltf_light.type == "spot") {
    light_specific_data =
        spot_light{.inner_cone_angle = gltf_light.spot.innerConeAngle,
                   .outer_cone_angle = gltf_light.spot.outerConeAngle};
  }

  return light_asset{.color = maybe_colour.value(),
                    .intensity = gltf_light.intensity,
                    .range = gltf_light.range};

}

}  // namespace wunder