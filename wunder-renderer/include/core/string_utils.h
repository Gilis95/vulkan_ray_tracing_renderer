#ifndef WUNDER_STRING_UTILS_H
#define WUNDER_STRING_UTILS_H

#include <cstdint>
#include <string>

namespace wunder::string::utils {

std::string bytes_to_string(std::uint64_t bytes);
bool ichar_equals(char a, char b);

}

#endif  // WUNDER_STRING_UTILS_H
