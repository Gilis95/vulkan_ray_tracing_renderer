//
// Created by christian on 8/26/24.
//

#ifndef WUNDER_LIGHT_ASSET_H
#define WUNDER_LIGHT_ASSET_H

#include <variant>

#include "glm/vec4.hpp"

namespace wunder {
struct spot_light {
  double inner_cone_angle;
  double outer_cone_angle;
};

struct directional_light {};

struct point_light {};

using specific_light_type_data =
    std::variant<spot_light, directional_light, point_light>;

struct light_asset {
  glm::vec4 color;
  double intensity{1.0};
  double range{0.0};  // 0.0 = infinite
  specific_light_type_data specific_data;
};
}  // namespace wunder
#endif  // WUNDER_LIGHT_ASSET_H
