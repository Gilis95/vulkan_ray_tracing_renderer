
#ifndef WUNDER_VULKAN_INDEX_BUFFER_H
#define WUNDER_VULKAN_INDEX_BUFFER_H

#include "gla/vulkan/vulkan_buffer_fwd.h"
#include "core/wunder_memory.h"

namespace wunder{
struct mesh_asset;
}

namespace wunder::vulkan {

class index_buffer {
 public:
  static unique_ptr<storage_buffer> create(const mesh_asset& asset);
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_INDEX_BUFFER_H
