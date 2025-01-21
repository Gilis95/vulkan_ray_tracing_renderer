#ifndef WUNDER_ENVIRONMENT_MAP_SERIALIZER_H
#define WUNDER_ENVIRONMENT_MAP_SERIALIZER_H

#include <cstdint>

#include "assets/asset_types.h"
namespace wunder {
class asset_storage;

class environment_map_serializer final {
  struct input_data {
    int32_t m_width, m_height, m_components;
    float* m_pixels_ptr;
  };

 public:
  static asset_serialization_result_codes import_asset(input_data input, asset_storage& out_storage);
};
}  // namespace wunder
#endif  // WUNDER_ENVIRONMENT_MAP_SERIALIZER_H
