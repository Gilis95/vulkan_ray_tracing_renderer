//
// Created by christian on 8/26/24.
//

#ifndef WUNDER_LIGHT_COMPONENT_H
#define WUNDER_LIGHT_COMPONENT_H

#include <glm/vec4.hpp>
namespace wunder {
struct spot_light {
  double inner_cone_angle;
  double outer_cone_angle;
};

struct directional_light {};

struct point_light {};

using speecific_light_type_data =
    std::variant<spot_light, directional_light, point_light>;

struct light_component {
  glm::vec4 color;
  double intensity{1.0};
  double range{0.0};  // 0.0 = infinite
  speecific_light_type_data specific_data;
};
}  // namespace wunder
#endif  // WUNDER_LIGHT_COMPONENT_H
