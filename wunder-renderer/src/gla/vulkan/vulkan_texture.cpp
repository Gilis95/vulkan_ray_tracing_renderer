#include "gla/vulkan/vulkan_texture.h"

#include "assets/components/texture_asset.h"
#include "core/vector_map.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_memory_allocator.h"

namespace {

std::unordered_map<wunder::texture_filter_type, VkFilter> s_filters = {
    {wunder::texture_filter_type::NEAREST, VK_FILTER_NEAREST},  // NEAREST
    {wunder::texture_filter_type::LINEAR, VK_FILTER_LINEAR},    // LINEA
};
std::unordered_map<wunder::mipmap_mode_type, VkSamplerMipmapMode> s_mip_map{
    {wunder::mipmap_mode_type::NEAREST,
     VK_SAMPLER_MIPMAP_MODE_NEAREST},  // NEAREST
    {wunder::mipmap_mode_type::LINEAR,
     VK_SAMPLER_MIPMAP_MODE_LINEAR}  // LINEAR_MIPMAP_LINEAR
};
std::unordered_map<wunder::address_mode_type, VkSamplerAddressMode> s_address_mode{
    {wunder::address_mode_type::CLAMP_TO_EDGE,
     VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE},
    {wunder::address_mode_type::MIRRORED_REPEAT,
     VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT},
    {wunder::address_mode_type::REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT}};
}  // namespace

namespace wunder {

vulkan_texture::vulkan_texture(const texture_asset& asset) {
  auto& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  auto vulkan_logical_device = device.get_vulkan_logical_device();;

  std::string name = "texture";

  VkFormat image_format =
      allocate_image( name);
  create_image_view(name, image_format);

  try_create_sampler(asset, vulkan_logical_device, name);

  bind_texture_data(asset);
}

vulkan_texture::~vulkan_texture() = default;

void vulkan_texture::try_create_sampler(const texture_asset& asset,
                                        VkDevice vulkan_logical_device,
                                        const std::string& name) {
  ReturnUnless(asset.m_sampler.has_value());

  auto& model_sampler = asset.m_sampler.value();

  VkSamplerCreateInfo sampler_create_info = {};
  sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_create_info.maxAnisotropy = 1.0f;

  sampler_create_info.addressModeU =
      s_address_mode[model_sampler.m_address_mode_u];
  sampler_create_info.addressModeV =
      s_address_mode[model_sampler.m_address_mode_v];

  sampler_create_info.magFilter = s_filters[model_sampler.m_mag_filter];
  sampler_create_info.minFilter = s_filters[model_sampler.m_min_filter];

  //TODO::
  sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

  sampler_create_info.mipLodBias = 0.0f;
  sampler_create_info.minLod = 0.0f;
  sampler_create_info.maxLod = 100.0f;
  sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

  VK_CHECK_RESULT(vkCreateSampler(vulkan_logical_device, &sampler_create_info,
                                  nullptr, &(m_image_info.m_sampler)));

  set_debug_utils_object_name(vulkan_logical_device, VK_OBJECT_TYPE_SAMPLER,
                              std::format("{} default sampler", name),
                              m_image_info.m_sampler);
}

void vulkan_texture::create_image_view( const std::string& name,
    const VkFormat& image_format) {  // Create a default image view
  auto& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  auto vulkan_logical_device = device.get_vulkan_logical_device();

  VkImageViewCreateInfo image_view_create_info = {};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.format = image_format;
  image_view_create_info.flags = 0;
  image_view_create_info.subresourceRange = {};
  image_view_create_info.subresourceRange.aspectMask =
      VK_IMAGE_ASPECT_COLOR_BIT;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount =
      VK_REMAINING_ARRAY_LAYERS;
  image_view_create_info.image = m_image_info.m_image;

  VK_CHECK_RESULT(vkCreateImageView(vulkan_logical_device,
                                    &image_view_create_info, nullptr,
                                    &(m_image_info.m_image_view)));

  set_debug_utils_object_name(vulkan_logical_device, VK_OBJECT_TYPE_IMAGE_VIEW,
                              std::format("{} default image view", name),
                              m_image_info.m_image_view);
}

VkFormat vulkan_texture::allocate_image(const std::string& name) {
  auto& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  auto vulkan_logical_device = device.get_vulkan_logical_device();
  auto& allocator = vulkan_context.get_resource_allocator();

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;
  VkFormat image_format = VK_FORMAT_R8G8B8A8_UNORM;

  VkImageCreateInfo image_create_info = {};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = VK_IMAGE_TYPE_2D;
  image_create_info.format = image_format;
  image_create_info.extent.width = 1;
  image_create_info.extent.height = 1;
  image_create_info.extent.depth = 1;
  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_create_info.usage = VK_IMAGE_USAGE_SAMPLED_BIT |
                            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                            VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  m_image_info.m_memory_alloc =
      allocator.allocate_image(image_create_info, memoryUsage,
                               m_image_info.m_image, &m_gpu_allocation_size);
  set_debug_utils_object_name(vulkan_logical_device, VK_OBJECT_TYPE_IMAGE,
                              std::format("{} default image view", name),
                              m_image_info.m_image);
  return image_format;
}

void vulkan_texture::bind_texture_data(const texture_asset& asset) {
  const auto& texture_data = asset.m_texture_data;
  ReturnIf(texture_data.empty());

  auto& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  auto vulkan_logical_device = device.get_vulkan_logical_device();
  auto& command_pool = device.get_command_pool();
  auto& allocator = vulkan_context.get_resource_allocator();

  VkMemoryAllocateInfo memAllocInfo{};
  memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

  // Create staging buffer
  VkBufferCreateInfo buffer_create_info{};
  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = texture_data.size();
  buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  VkBuffer stagingBuffer;
  VmaAllocation stagingBufferAllocation = allocator.allocate_buffer(
      buffer_create_info, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer);

  // Copy data to staging buffer
  auto* dest_data = allocator.map_memory<uint8_t>(stagingBufferAllocation);

  memcpy(dest_data, texture_data.data(), texture_data.size());
  allocator.unmap_memory(stagingBufferAllocation);

  VkCommandBuffer copy_cmd = command_pool.get_current_compute_command_buffer();

  // Image memory barriers for the texture image

  // The sub resource range describes the regions of the image that will be
  // transitioned using the memory barriers below
  VkImageSubresourceRange subresource_range = {};
  // Image only contains color data
  subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  // Start at first mip level
  subresource_range.baseMipLevel = 0;
  subresource_range.levelCount = 1;
  subresource_range.layerCount = 1;

  // Transition the texture image layout to transfer target, so we can safely
  // copy our buffer data to it.
  VkImageMemoryBarrier image_memory_barrier{};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = m_image_info.m_image;
  image_memory_barrier.subresourceRange = subresource_range;
  image_memory_barrier.srcAccessMask = 0;
  image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

  // Insert a memory dependency at the proper pipeline stages that will execute
  // the image layout transition Source pipeline stage is host write/read
  // exection (VK_PIPELINE_STAGE_HOST_BIT) Destination pipeline stage is copy
  // command exection (VK_PIPELINE_STAGE_TRANSFER_BIT)
  vkCmdPipelineBarrier(copy_cmd, VK_PIPELINE_STAGE_HOST_BIT,
                       VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &image_memory_barrier);

  VkBufferImageCopy buffer_copy_region = {};
  buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  buffer_copy_region.imageSubresource.mipLevel = 0;
  buffer_copy_region.imageSubresource.baseArrayLayer = 0;
  buffer_copy_region.imageSubresource.layerCount = 1;
  buffer_copy_region.imageExtent.width = asset.m_width;
  buffer_copy_region.imageExtent.height = asset.m_height;
  buffer_copy_region.imageExtent.depth = 1;
  buffer_copy_region.bufferOffset = 0;

  // Copy mip levels from staging buffer
  vkCmdCopyBufferToImage(copy_cmd, stagingBuffer, m_image_info.m_image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &buffer_copy_region);
}

}  // namespace wunder