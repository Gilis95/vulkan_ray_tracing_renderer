#include "wunder_application.h"

#include <application_properties.h>
#include <gla/graphic_layer_abstraction_factory.h>
#include <gla/renderer_properties.h>
#include <window/window_properties.h>

#include <entry_point.h>

#include <utility>

namespace wunder {

wunder_application::wunder_application(application_properties&& properties)
    : application(std::move(properties)) {}

void wunder_application::init_internal() {}

/////////////////////////////////////////////////////////////////////////////////////////
application *create_application() {
  auto app_properties = application_properties{
      "wunder", "123",
      window_properties{"Wunder Application", 1280, 720, window_type::glfw},
      renderer_properties{gla_type::Vulkan}
  };

  return new wunder_application(std::move(app_properties));
}
}  // namespace wunder
