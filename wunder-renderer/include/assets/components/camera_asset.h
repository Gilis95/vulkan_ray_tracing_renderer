//
// Created by christian on 8/22/24.
//

#ifndef WUNDER_CAMERA_ASSET_H
#define WUNDER_CAMERA_ASSET_H
#include "glm/vec3.hpp"

namespace wunder {
struct perspective_camera_component {
  double aspect_ratio;
  double fov;
};

struct camera_asset {
  glm::vec3 eye{0, 0, 0};
  glm::vec3 center{0, 0, 0};
  glm::vec3 up{0, 1, 0};

  perspective_camera_component m_perspective_camera_data;
};
}  // namespace wunder
#endif  // WUNDER_CAMERA_ASSET_H
