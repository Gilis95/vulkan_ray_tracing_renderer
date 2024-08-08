//
// Created by christian on 8/7/24.
//

#ifndef WUNDER_RENDERER_CAPABILITIES_H
#define WUNDER_RENDERER_CAPABILITIES_H

#include <string>

namespace wunder {
struct renderer_capabilities {
 public:
  ~renderer_capabilities() = default;

  std::string vendor;
  std::string device;
  std::string version;

  int MaxSamples = 0;
  float MaxAnisotropy = 0.0f;
  int MaxTextureUnits = 0;
};
}  // namespace wunder
#endif  // WUNDER_RENDERER_CAPABILITIES_H
