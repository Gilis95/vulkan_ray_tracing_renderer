//
// Created by christian on 9/22/24.
//

#ifndef WUNDER_VULKAN_VERTEX_BUFFER_H
#define WUNDER_VULKAN_VERTEX_BUFFER_H

#include "gla/vulkan/vulkan_device_buffer.h"
namespace wunder {
struct mesh_asset;
}

namespace wunder::vulkan {
class vertex_buffer {
 public:
  static buffer create(const mesh_asset& asset);
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_VERTEX_BUFFER_H
