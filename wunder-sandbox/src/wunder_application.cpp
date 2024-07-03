#include "wunder_application.h"

#include <entry_point.h>

namespace wunder {

wunder_application::wunder_application(application_properties properties)
    : application(properties) {}

void wunder_application::init_internal() {}

/////////////////////////////////////////////////////////////////////////////////////////
application *create_application() {
  return new wunder_application(application_properties{
      "wunder", "123",
      window_properties{"Wunder Application", 1280, 720, window_type::glfw}});
}
}  // namespace wunder
