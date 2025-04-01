#include "core/project.h"

#include "assets/asset_manager.h"
#include "include/gla/vulkan/scene/vulkan_scene.h"
#include "scene/scene_manager.h"

namespace wunder {
/////////////////////////////////////////////////////////////////////////////////////////
project::project() = default;

/////////////////////////////////////////////////////////////////////////////////////////
project::~project() = default;

/////////////////////////////////////////////////////////////////////////////////////////
project& project::instance() {
  static project s_instance;
  return s_instance;
}

/////////////////////////////////////////////////////////////////////////////////////////
void project::update(time_unit dt) {
  m_asset_manager->update(dt);
}

/////////////////////////////////////////////////////////////////////////////////////////
void project::initialize() {
  m_scene_manager = make_unique<scene_manager>();
  m_asset_manager = make_unique<asset_manager>();
}

/////////////////////////////////////////////////////////////////////////////////////////
void project::shutdown() {
  AssertLogUnless(m_asset_manager.release());
  AssertLogUnless(m_scene_manager.release());
}

/////////////////////////////////////////////////////////////////////////////////////////
asset_manager& project::get_asset_manager() {
  return *m_asset_manager;
}

/////////////////////////////////////////////////////////////////////////////////////////
scene_manager& project::get_scene_manager() {
  return *m_scene_manager;
}
}  // namespace wunder
