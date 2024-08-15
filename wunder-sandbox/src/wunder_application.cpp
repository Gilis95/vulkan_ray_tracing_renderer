#include "wunder_application.h"

#include <application_properties.h>
#include <core/wunder_filesystem.h>
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
  auto current_path = std::filesystem::current_path();
  wunder::wunder_filesystem::instance().set_work_dir((std::filesystem::absolute(current_path / ".." / "..")));

  auto app_properties = application_properties{
      "wunder", "123",
      window_properties{"Wunder Application", 1280, 720, window_type::glfw},
      renderer_properties {
          .m_driver = driver::Vulkan,
          .m_renderer = renderer_type::RAY_TRACE,
          .m_gpu_to_use = gpu_to_use::Dedicated,
          .m_enable_validation = true
      }
  };

  return new wunder_application(std::move(app_properties));
}
}  // namespace wunder
