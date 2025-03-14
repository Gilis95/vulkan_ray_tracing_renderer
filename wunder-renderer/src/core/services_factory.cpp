#include "core/services_factory.h"

#include "camera/camera.h"
#include "core/input_manager.h"

namespace wunder {
service_factory::service_factory() = default;
service_factory::~service_factory() = default;

service_factory& service_factory::instance() {
  static service_factory factory;
  return factory;
}

void service_factory::initialize() {
  /**
   * TODO:: Currently order definitions matters, because we don't have priority
   * in event manager
   **/

  m_input_manager = make_unique<input_manager>();
  m_camera = make_unique<camera>();
}

void service_factory::shutdown() {
  m_input_manager.release();
  m_camera.release();
}


void service_factory::update(time_unit dt){
  m_camera->update(dt);
  m_input_manager->update(dt);
}


camera& service_factory::get_camera() { return *m_camera; }

input_manager& service_factory::get_input_manager() {
  return *m_input_manager;
}

}  // namespace wunder
