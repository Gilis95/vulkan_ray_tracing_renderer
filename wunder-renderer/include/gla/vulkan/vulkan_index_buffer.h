
#ifndef WUNDER_VULKAN_INDEX_BUFFER_H
#define WUNDER_VULKAN_INDEX_BUFFER_H

namespace wunder{
struct mesh_asset;
}

namespace wunder::vulkan {
class buffer;

class index_buffer {
 public:
  static buffer create(const mesh_asset& asset);
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_INDEX_BUFFER_H
