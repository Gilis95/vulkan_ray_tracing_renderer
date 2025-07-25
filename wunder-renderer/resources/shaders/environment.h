#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "macros.h"


// Environment acceleration structure - computed in hdr_sampling
struct EnvAccel {
  uint alias;
  float q;
  float pdf;
  float aliasPdf;
};

struct SunAndSky {
  vec3 rgb_unit_conversion;
  float multiplier;

  float haze;
  float redblueshift;
  float saturation;
  float horizon_height;

  vec3 ground_color;
  float horizon_blur;

  vec3 night_color;
  float sun_disk_intensity;

  vec3 sun_direction;
  float sun_disk_scale;

  float sun_glow_intensity;
  int y_is_up;
  int physically_scaled_sun;
  int in_use;
};


#endif //ENVIRONMENT_H
