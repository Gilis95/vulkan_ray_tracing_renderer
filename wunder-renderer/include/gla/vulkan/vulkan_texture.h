#ifndef WUNDER_VULKAN_TEXTURE_H
#define WUNDER_VULKAN_TEXTURE_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include "core/wunder_memory.h"
#include "vulkan_memory_allocator.h"

namespace wunder {
struct texture_asset;
}

namespace wunder::vulkan {
struct vulkan_image_info {
  VkImage m_image = VK_NULL_HANDLE;
  VkImageView m_image_view = VK_NULL_HANDLE;
  VkSampler m_sampler = VK_NULL_HANDLE;
  VmaAllocation m_memory_alloc = VK_NULL_HANDLE;
};

class renderer;

class texture {
 public:
  texture(const texture_asset& asset);
  ~texture();

 public:
  void bind(renderer& renderer);

 private:
  VkFormat allocate_image(const std::string& name);
  void create_image_view(const std::string& name, const VkFormat& image_format);
  void try_create_sampler(const texture_asset& asset,
                          VkDevice vulkan_logical_device,
                          const std::string& name);

  void bind_texture_data(const texture_asset& asset);

 private:
  vulkan_image_info m_image_info;
  VkDeviceSize m_gpu_allocation_size = 0;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_TEXTURE_H
