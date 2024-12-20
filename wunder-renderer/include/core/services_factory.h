#ifndef WUNDER_SERVICES_FACTORY_H
#define WUNDER_SERVICES_FACTORY_H

#include "core/wunder_memory.h"
#include "time_unit.h"

namespace wunder {
class camera;
class input_manager;
class service_factory final {
 private:
  service_factory();
  ~service_factory();

 public:
  static service_factory& instance();

  void initialize();
 public:
  void update(time_unit dt);
 public:
  camera& get_camera();
  input_manager& get_input_manager();

 private:
  unique_ptr<camera> m_camera;
  unique_ptr<input_manager> m_input_manager;
};
}  // namespace wunder
#endif  // WUNDER_SERVICES_FACTORY_H
