//
// Created by christian on 8/13/24.
//

#ifndef WUNDER_WUNDER_FILESYSTEM_H
#define WUNDER_WUNDER_FILESYSTEM_H

#include <filesystem>

namespace wunder {
class wunder_filesystem {
 private:
  wunder_filesystem();
 public:
  static wunder_filesystem& instance();
 public:
  void set_work_dir(std::filesystem::path work_dir);

  std::filesystem::path resolve_path(const std::filesystem::path& resource_path);
 private:
  std::filesystem::path m_work_dir;
};
}  // namespace wunder
#endif  // WUNDER_WUNDER_FILESYSTEM_H
