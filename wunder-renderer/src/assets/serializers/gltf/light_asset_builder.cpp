#include "assets/serializers/gltf/light_asset_builder.h"

#include "include/assets/light_asset.h"
#include "tinygltf/tinygltf_utils.h"

namespace wunder {

light_asset_builder::light_asset_builder(const tinygltf::Light& gltf_light)
    : gltf_light(gltf_light) {}

std::optional<light_asset> light_asset_builder::build() {
  auto maybe_colour =
      tinygltf::utils::vector_to_glm<glm::vec4>(gltf_light.color);
  if(!maybe_colour.has_value()) {
    maybe_colour = glm::vec4(1.f);
  }
  specific_light_type_data light_specific_data;
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
                     .range = gltf_light.range,
                     .specific_data = light_specific_data};
}

}  // namespace wunder