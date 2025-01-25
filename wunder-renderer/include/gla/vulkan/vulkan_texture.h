#ifndef WUNDER_VULKAN_TEXTURE_H
#define WUNDER_VULKAN_TEXTURE_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_shader_types.h"
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

class rtx_renderer;

template <typename base_texture>
class texture : public base_texture {
 public:
  texture(descriptor_build_data build_data, VkFormat image_format,
          std::uint32_t width, std::uint32_t height);
  texture(descriptor_build_data build_data, const texture_asset& asset);
  ~texture();

 public:
  void add_descriptor_to(rtx_renderer& renderer) override;

 private:
  void allocate_image(const std::string& name, VkFormat image_format,
                      std::uint32_t width, std::uint32_t height);
  void create_image_view(const std::string& name, VkFormat image_format);

  void try_screate_sampler();
  void try_create_sampler(const texture_asset& asset, const std::string& name);

  void bind_texture(VkImageLayout target_layout);
  void bind_texture_data(const texture_asset& asset,
                         VkImageLayout target_layout);

  void transit_image_layout(VkCommandBuffer& command_buffer,
                            VkImageLayout old_layout, VkImageLayout new_layout);

  [[nodiscard]] static std::string generate_next_texture_name();

 private:
  descriptor_build_data m_descriptor_build_data;
  vulkan_image_info m_image_info;
  VkDeviceSize m_gpu_allocation_size = 0;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_TEXTURE_H
