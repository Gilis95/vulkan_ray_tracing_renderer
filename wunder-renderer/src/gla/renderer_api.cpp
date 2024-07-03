#include "gla/renderer_api.h"

namespace wunder {
renderer_api::~renderer_api() = default;

void renderer_api::init(const renderer_properties &properties) {
  init_internal(properties);
}
}  // namespace wunder