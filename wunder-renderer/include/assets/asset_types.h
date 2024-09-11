#ifndef WUNDER_ASSET_TYPES_H
#define WUNDER_ASSET_TYPES_H

#include <cstdint>

namespace wunder {
enum class asset_serialization_result_codes {
  ok = 0,
  warning = 1000,
  error = 10000
};

using asset_handle = std::uint32_t;
}
#endif  // WUNDER_ASSET_TYPES_H
