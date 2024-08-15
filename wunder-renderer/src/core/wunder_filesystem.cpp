#include "core/wunder_filesystem.h"

#include "core/wunder_macros.h"

namespace wunder {
wunder_filesystem wunder_filesystem::s_instance;

wunder_filesystem::wunder_filesystem() = default;

wunder_filesystem& wunder_filesystem::instance() { return s_instance; }

void wunder_filesystem::set_work_dir(std::filesystem::path work_dir) {
  m_work_dir = std::filesystem::canonical(work_dir);
}

std::filesystem::path wunder_filesystem::resolve_path(
    const std::filesystem::path& resource_path) {
  ReturnIf(m_work_dir.empty(), std::filesystem::absolute(resource_path));
  // failsafe path has been already resolved or it has been provided as a full
  // path
  ReturnIf(resource_path.string().rfind(m_work_dir.string(), 0) == 0,
           std::filesystem::absolute(resource_path));

  return m_work_dir / resource_path;
}
}  // namespace wunder