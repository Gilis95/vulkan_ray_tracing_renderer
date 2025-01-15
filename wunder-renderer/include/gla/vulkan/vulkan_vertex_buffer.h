//
// Created by christian on 9/22/24.
//

#ifndef WUNDER_VULKAN_VERTEX_BUFFER_H
#define WUNDER_VULKAN_VERTEX_BUFFER_H

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"
namespace wunder {
struct mesh_asset;
}

namespace wunder::vulkan {
class vertex_buffer {
 public:
  static unique_ptr<storage_buffer> create(const mesh_asset& asset);
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_VERTEX_BUFFER_H
