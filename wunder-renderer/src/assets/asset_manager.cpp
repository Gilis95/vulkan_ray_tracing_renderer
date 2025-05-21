#include "assets/asset_manager.h"

#include <stb_image.h>
#include <tiny_gltf.h>

#include "assets/asset_importer_task.h"
#include "assets/serializers/environment_map_serializer.h"
#include "assets/serializers/gltf/gltf_asset_importer.h"
#include "core/task_executor.h"
#include "core/wunder_filesystem.h"
#include "event/event_handler.hpp"

namespace wunder {

asset_manager::asset_manager()
    : event_handler(),
      m_gltf(std::make_unique<tinygltf::TinyGLTF>()),
      m_asset_importer(std::make_unique<gltf_asset_importer>(m_asset_storage)),
      m_asset_importer_executor(make_unique<task_executor>(1)) {}

asset_manager::~asset_manager() = default;

void asset_manager::update(time_unit dt) {
  m_asset_importer_executor->update(dt);
}

asset_serialization_result_codes asset_manager::import_asset(
    const std::filesystem::path &asset) {
  auto scene_real_path = wunder_filesystem::instance().resolve_path(asset);
  AssertReturnUnless(std::filesystem::exists(scene_real_path),
                     asset_serialization_result_codes::error);

  AssertReturnUnless(scene_real_path.has_extension(),
                     asset_serialization_result_codes::error);

  unique_ptr<asset_importer_task> task = make_unique<asset_importer_task>(
      *m_gltf, *m_asset_importer, scene_real_path);
  AssertReturnUnless(
      task->is_file_supported(),
      asset_serialization_result_codes::not_supported_format_error);

  m_asset_importer_executor->enqueue(std::move(task));

  return asset_serialization_result_codes::scheduled;
}

asset_serialization_result_codes asset_manager::import_environment_map(
    const std::filesystem::path &asset) {
  auto environment_map_real_path =
      wunder_filesystem::instance().resolve_path(asset);
  AssertReturnUnless(std::filesystem::exists(environment_map_real_path),
                     asset_serialization_result_codes::error);

  int32_t width{0};
  int32_t height{0};
  int32_t component{0};
  int32_t required_components = STBI_rgb_alpha;

  float *pixels = stbi_loadf(environment_map_real_path.c_str(), &width, &height,
                             &component, required_components);

  return environment_map_serializer::import_asset(
      {.m_width = static_cast<uint32_t>(width),
       .m_height = static_cast<uint32_t>(height),
       .m_components = static_cast<uint32_t>(required_components),
       .m_pixels_ptr = pixels},
      m_asset_storage);
}

void asset_manager::on_event(const event::file_dropped &event) {
  import_asset(event.m_path);
}

}  // namespace wunder