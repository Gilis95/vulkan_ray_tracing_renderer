#include "core/string_utils.h"

namespace wunder::string::utils {

std::string bytes_to_string(std::uint64_t bytes) {
  constexpr uint64_t GB = 1024 * 1024 * 1024;
  constexpr uint64_t MB = 1024 * 1024;
  constexpr uint64_t KB = 1024;

  char buffer[32 + 1]{};

  if (bytes >= GB)
    snprintf(buffer, 32, "%.2f GB", static_cast<float>(bytes) / static_cast<float>(GB));
  else if (bytes >= MB)
    snprintf(buffer, 32, "%.2f MB", static_cast<float>(bytes) / static_cast<float>(MB));
  else if (bytes >= KB)
    snprintf(buffer, 32, "%.2f KB", static_cast<float>(bytes) / static_cast<float>(KB));
  else
    snprintf(buffer, 32, "%.2f bytes", static_cast<float>(bytes));

  return std::string(buffer);
}

bool ichar_equals(char a, char b) {
  return std::tolower(static_cast<unsigned char>(a)) ==
         std::tolower(static_cast<unsigned char>(b));
}

}