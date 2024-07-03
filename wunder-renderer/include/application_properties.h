//
// Created by christian on 7/3/24.
//

#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_APPLICATION_PROPERTIES_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_APPLICATION_PROPERTIES_H_

#include "gla/renderer_properties.h"
#include "window/window_properties.h"

namespace wunder {
struct application_properties {
  std::string m_debug_name;
  std::string m_debug_version;
  window_properties m_window_properties;
  renderer_properties m_renderer_properties;
};
}  // namespace wunder
#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_APPLICATION_PROPERTIES_H_
