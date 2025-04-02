#ifndef SCENE_LOAD_TASK_H
#define SCENE_LOAD_TASK_H

#include "core/async_task.h"
#include "scene/scene_types.h"

namespace wunder {
class scene_asset;
namespace vulkan {
class scene;
}
}  // namespace wunder

namespace wunder {
class scene_load_task : public async_task {
 public:
  scene_load_task(scene_id id, vulkan::scene& scene, const scene_asset& input_scene_asset);

 private:
  void run() override;

  void execute_on_main_thread() override;

 private:
  scene_id m_id;
  vulkan::scene& m_out_scene;
  const scene_asset& m_input_scene_asset;
};
}  // namespace wunder
#endif  // SCENE_LOAD_TASK_H
