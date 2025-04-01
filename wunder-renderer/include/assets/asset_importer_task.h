#ifndef BASE_ASSET_IMPORTER_TASK_H
#define BASE_ASSET_IMPORTER_TASK_H
#include <tiny_gltf.h>

#include <filesystem>
#include <functional>
#include <unordered_map>

#include "asset_types.h"
#include "core/async_task.h"
#include "core/wunder_memory.h"

namespace wunder {
class gltf_asset_importer;

class asset_importer_task : public async_task {
 public:
  asset_importer_task(tinygltf::TinyGLTF& gltf,
                      gltf_asset_importer& asset_importer,
                      std::filesystem::path asset_path);

 public:
  bool is_file_supported() const;

 private:
  void run() override;

  void execute_on_main_thread() override;

 private:
  tinygltf::TinyGLTF& m_gltf;
  gltf_asset_importer& m_asset_importer;
  std::filesystem::path m_asset_path;

  tinygltf::Model gltf_model;
  asset_serialization_result_codes m_result_code;

  std::unordered_map<
      std::string,
      std::function<bool(tinygltf::Model*, std::string*, std::string*,
                         const std::string&, unsigned int)>>
      m_load_fns;
};
}  // namespace wunder
#endif  // BASE_ASSET_IMPORTER_TASK_H
