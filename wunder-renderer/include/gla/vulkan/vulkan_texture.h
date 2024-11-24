#ifndef WUNDER_VULKAN_TEXTURE_H
#define WUNDER_VULKAN_TEXTURE_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include "core/wunder_memory.h"
#include "vulkan_memory_allocator.h"

namespace wunder {
struct vulkan_image_info {
  VkImage m_image = VK_NULL_HANDLE;
  VkImageView m_image_view = VK_NULL_HANDLE;
  VkSampler m_sampler = VK_NULL_HANDLE;
  VmaAllocation m_memory_alloc = VK_NULL_HANDLE;
};

struct texture_asset;

class vulkan_texture {
 public:
  vulkan_texture(const texture_asset& asset);
  ~vulkan_texture();

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
}  // namespace wunder
#endif  // WUNDER_VULKAN_TEXTURE_H
