
#ifndef WUNDER_VULKAN_INDEX_BUFFER_H
#define WUNDER_VULKAN_INDEX_BUFFER_H

namespace wunder {
class vulkan_buffer;
struct mesh_asset;

class vulkan_index_buffer {
 public:
  static vulkan_buffer create(const mesh_asset& asset);
};
}
#endif  // WUNDER_VULKAN_INDEX_BUFFER_H
