#ifndef WUNDER_VULKAN_TEXTURE_H
#define WUNDER_VULKAN_TEXTURE_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_shader_types.h"

namespace wunder {
struct texture_asset;
}

namespace wunder::vulkan {
class descriptor_set_manager;

struct vulkan_image_info {
  ~vulkan_image_info();

  VkImage m_image = VK_NULL_HANDLE;
  VkImageView m_image_view = VK_NULL_HANDLE;
  VkSampler m_sampler = VK_NULL_HANDLE;
  VmaAllocation m_memory_alloc = VK_NULL_HANDLE;
};

class base_renderer;

template <typename base_texture>
class texture : public base_texture {
 public:
  texture(descriptor_build_data build_data, VkFormat image_format,
          std::uint32_t width, std::uint32_t height);
  texture(descriptor_build_data build_data, const texture_asset& asset);

  template <typename other_base_texture>
  explicit texture(const texture<other_base_texture>& other);

  ~texture();

 public:
  void add_descriptor_to(descriptor_set_manager& target) override;

 private:
  void allocate_image(const std::string& name, VkFormat image_format,
                      std::uint32_t width, std::uint32_t height);
  void create_image_view(const std::string& name, VkFormat image_format);

  void try_create_sampler();
  bool fill_sampler_create_info_from(
      const texture_asset& asset, VkSamplerCreateInfo& out_sampler_create_info);
  void try_create_sampler(const texture_asset& asset, const std::string& name);

  void bind_texture(VkImageLayout target_layout);
  void bind_texture_data(const texture_asset& asset,
                         VkImageLayout target_layout);

  void transit_image_layout(VkCommandBuffer& command_buffer,
                            VkImageLayout old_layout, VkImageLayout new_layout);

  [[nodiscard]] static std::string generate_next_texture_name();

 public:
  [[nodiscard]] std::shared_ptr<vulkan_image_info> get_image_info() const {
    return m_image_info;
  }

  [[nodiscard]] VkDeviceSize get_gpu_allocation_size() const {
    return m_gpu_allocation_size;
  }

  [[nodiscard]] const descriptor_build_data& get_descriptor_build_data() const {
    return m_descriptor_build_data;
  }

 private:
  std::shared_ptr<vulkan_image_info> m_image_info;
  VkDeviceSize m_gpu_allocation_size = 0;
  int32_t m_mip_levels = 0;
  descriptor_build_data m_descriptor_build_data;
};

template <typename base_texture>
template <typename other_base_texture>
texture<base_texture>::texture(const texture<other_base_texture>& other)
    : m_descriptor_build_data(other.get_descriptor_build_data()),
      m_image_info(other.get_image_info()),
      m_gpu_allocation_size(other.get_gpu_allocation_size()) {
  base_texture::m_descriptor = other.m_descriptor;
}

}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_TEXTURE_H
