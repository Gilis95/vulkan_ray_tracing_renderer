#ifndef WUNDER_PROJECT_H
#define WUNDER_PROJECT_H

#include "core/wunder_memory.h"
#include "time_unit.h"

namespace wunder {
class scene_manager;
class asset_manager;

class project final {
 private:
  project();

 public:
  ~project();

 public:
  static project& instance();

 public:
  void update(time_unit dt);

  void initialize();
  void shutdown();

 public:
  asset_manager& get_asset_manager();
  scene_manager& get_scene_manager();

 private:
  unique_ptr<scene_manager> m_scene_manager;
  unique_ptr<asset_manager> m_asset_manager;
};
}  // namespace wunder
#endif  // WUNDER_PROJECT_H
