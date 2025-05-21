#ifndef FILE_EVENTS_H
#define FILE_EVENTS_H

#include <filesystem>

namespace wunder::event{
struct file_dropped{
  std::filesystem::path m_path;
};
}
#endif //FILE_EVENTS_H
