#include "gla/vulkan/vulkan_texture.h"

#include "core/vector_map.h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "include/assets/texture_asset.h"
#include "include/gla/vulkan/ray-trace/vulkan_rtx_renderer.h"

namespace {

uint32_t texture_counter = 0;

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

std::unordered_map<wunder::address_mode_type, VkSamplerAddressMode>
    s_address_mode{
        {wunder::address_mode_type::CLAMP_TO_EDGE,
         VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE},
        {wunder::address_mode_type::MIRRORED_REPEAT,
         VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT},
        {wunder::address_mode_type::REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT}};

std::unordered_map<VkImageLayout, VkPipelineStageFlags>
    s_layout_to_pipeline_stage{
        {VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT},

        {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_PIPELINE_STAGE_TRANSFER_BIT},
        {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
        {VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT},
        {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
         VK_PIPELINE_STAGE_ALL_COMMANDS_BIT},
        {VK_IMAGE_LAYOUT_PREINITIALIZED, VK_PIPELINE_STAGE_HOST_BIT},
        {VK_IMAGE_LAYOUT_UNDEFINED, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT}};

std::unordered_map<VkImageLayout, VkAccessFlags> s_layout_to_access_flags{
    {VK_IMAGE_LAYOUT_PREINITIALIZED, VK_ACCESS_HOST_WRITE_BIT},
    {VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_ACCESS_TRANSFER_WRITE_BIT},
    {VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_ACCESS_TRANSFER_READ_BIT},
    {VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
     VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT},
    {VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
     VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT},
    {VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_ACCESS_SHADER_READ_BIT},
};

std::unordered_map<wunder::border_colour, VkBorderColor> s_border_colour_map{
    {wunder::border_colour::FLOAT_TRANSPARENT_BLACK,
     VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK},
    {wunder::border_colour::INT_TRANSPARENT_BLACK,
     VK_BORDER_COLOR_INT_TRANSPARENT_BLACK},
    {wunder::border_colour::FLOAT_OPAQUE_BLACK,
     VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK},
    {wunder::border_colour::INT_OPAQUE_BLACK, VK_BORDER_COLOR_INT_OPAQUE_BLACK},
    {wunder::border_colour::FLOAT_OPAQUE_WHITE,
     VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE},
    {wunder::border_colour::INT_OPAQUE_WHITE, VK_BORDER_COLOR_INT_OPAQUE_WHITE},
};

VkPipelineStageFlags pipeline_stage_for_layout(VkImageLayout layout) {
  auto pipeline_stage_it = s_layout_to_pipeline_stage.find(layout);
  ReturnIf(pipeline_stage_it == s_layout_to_pipeline_stage.end(),
           VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

  return pipeline_stage_it->second;
}

VkAccessFlags access_flags_for_image_layout(VkImageLayout layout) {
  auto access_flags_it = s_layout_to_access_flags.find(layout);
  ReturnIf(access_flags_it == s_layout_to_access_flags.end(), VkAccessFlags());
  return access_flags_it->second;
}

inline uint32_t mip_levels(VkExtent2D extent) {
  return static_cast<uint32_t>(
             std::floor(std::log2(std::max(extent.width, extent.height)))) +
         1;
}

}  // namespace

namespace wunder::vulkan {

vulkan_image_info::~vulkan_image_info() {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vulkan_logical_device = device.get_vulkan_logical_device();
  auto& allocator = vulkan_context.mutable_resource_allocator();

  if (m_image_view != VK_NULL_HANDLE) {
    vkDestroyImageView(vulkan_logical_device, m_image_view, VK_NULL_HANDLE);
  }

  if (m_sampler != VK_NULL_HANDLE) {
    vkDestroySampler(vulkan_logical_device, m_sampler, VK_NULL_HANDLE);
  }

  if (m_image != VK_NULL_HANDLE) {
    allocator.destroy_image(m_image, m_memory_alloc);
  }
}

template <typename base_texture>
texture<base_texture>::texture(descriptor_build_data build_data,
                               VkFormat image_format, std::uint32_t width,
                               std::uint32_t height)
    : m_image_info(std::make_shared<vulkan_image_info>()),
      m_image_size(width, height),
      m_descriptor_build_data(std::move(build_data)) {
  std::string name = generate_next_texture_name();
  VkImageLayout target_layout = VK_IMAGE_LAYOUT_GENERAL;
  m_mip_levels = mip_levels({width, height});

  allocate_image(name, image_format, width, height);
  create_image_view(name, image_format);
  try_create_sampler();
  bind_texture(target_layout);

  base_texture::m_descriptor.imageLayout = target_layout;
}

template <typename base_texture>
texture<base_texture>::texture(descriptor_build_data build_data,
                               const texture_asset& asset)
    : m_image_info(std::make_shared<vulkan_image_info>()),
      m_image_size(asset.m_width, asset.m_height),
      m_descriptor_build_data(std::move(build_data)) {
  std::string name = generate_next_texture_name();
  m_mip_levels = 1;

  VkFormat image_format = asset.m_texture_data.get_image_format();
  VkImageLayout target_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  allocate_image(name, image_format, asset.m_width, asset.m_height);
  create_image_view(name, image_format);
  try_create_sampler(asset, name);
  bind_texture_data(asset, target_layout);

  base_texture::m_descriptor.imageLayout = target_layout;
}

template <typename base_texture>
texture<base_texture>::~texture() {
  if (m_image_info) {
    m_image_info.reset();
  }
}

template <typename base_texture>
void texture<base_texture>::add_descriptor_to(descriptor_set_manager& target) {
  ReturnUnless(m_descriptor_build_data.m_enabled);
  target.add_resource(m_descriptor_build_data.m_descriptor_name, *this);
}

template <typename base_texture>
void texture<base_texture>::generate_mip_levels(
    VkCommandBuffer command_buffer) {
  // Transfer the top level image to a layout 'eTransferSrcOptimal` and its
  // access to 'eTransferRead'
  VkImageMemoryBarrier barrier{};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.subresourceRange.levelCount = 1;
  barrier.image = m_image_info->m_image;
  barrier.oldLayout = base_texture::m_descriptor.imageLayout;
  barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  barrier.srcAccessMask =
      access_flags_for_image_layout(base_texture::m_descriptor.imageLayout);
  barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  vkCmdPipelineBarrier(
      command_buffer,
      pipeline_stage_for_layout(base_texture::m_descriptor.imageLayout),
      VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

  if (m_mip_levels > 1) {
    // transfer remaining mips to DST optimal
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.subresourceRange.baseMipLevel = 1;
    barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
    vkCmdPipelineBarrier(
        command_buffer,
        pipeline_stage_for_layout(base_texture::m_descriptor.imageLayout),
        VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  };

  int32_t mipWidth = m_image_size.width;
  int32_t mipHeight = m_image_size.height;

  for (uint32_t i = 1; i < m_mip_levels; i++) {
    VkImageBlit blit;
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1,
                          mipHeight > 1 ? mipHeight / 2 : 1, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(command_buffer, m_image_info->m_image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, m_image_info->m_image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                   VK_FILTER_LINEAR);

    // Next
    {
      // Transition the current miplevel into a eTransferSrcOptimal layout, to
      // be used as the source for the next one.
      barrier.subresourceRange.baseMipLevel = i;
      barrier.subresourceRange.levelCount = 1;
      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                           VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                           nullptr, 1, &barrier);
    }

    if (mipWidth > 1) {
      mipWidth /= 2;
    }

    if (mipHeight > 1) {
      mipHeight /= 2;
    }
  }

  // Transition all miplevels (now in VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) back
  // to currentLayout
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  barrier.newLayout = base_texture::m_descriptor.imageLayout;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
  barrier.dstAccessMask =
      access_flags_for_image_layout(base_texture::m_descriptor.imageLayout);
  vkCmdPipelineBarrier(
      command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
      pipeline_stage_for_layout(base_texture::m_descriptor.imageLayout), 0, 0,
      nullptr, 0, nullptr, 1, &barrier);
}

template <typename base_texture>
void texture<base_texture>::try_create_sampler() {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vulkan_logical_device = device.get_vulkan_logical_device();

  VkSamplerCreateInfo sampler_create_info{};
  memset(&sampler_create_info, 0, sizeof(sampler_create_info));
  sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_create_info.maxLod = std::numeric_limits<float>::max();

  VK_CHECK_RESULT(vkCreateSampler(vulkan_logical_device, &sampler_create_info,
                                  nullptr, &(m_image_info->m_sampler)));

  base_texture::m_descriptor.sampler = m_image_info->m_sampler;
};

template <typename base_texture>
void texture<base_texture>::try_create_sampler(const texture_asset& asset,
                                               const std::string& name) {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vulkan_logical_device = device.get_vulkan_logical_device();

  VkSamplerCreateInfo sampler_create_info = {};
  sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  sampler_create_info.maxAnisotropy = 0.0f;
  if (!fill_sampler_create_info_from(asset, sampler_create_info)) {
    sampler_create_info.minFilter = VK_FILTER_LINEAR;
    sampler_create_info.magFilter = VK_FILTER_LINEAR;
    sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  }

  sampler_create_info.mipLodBias = 0.0f;
  sampler_create_info.minLod = 0.0f;
  sampler_create_info.maxLod = std::numeric_limits<float>::max();
  sampler_create_info.unnormalizedCoordinates = VK_FALSE;

  VK_CHECK_RESULT(vkCreateSampler(vulkan_logical_device, &sampler_create_info,
                                  nullptr, &(m_image_info->m_sampler)));

  base_texture::m_descriptor.sampler = m_image_info->m_sampler;

  set_debug_utils_object_name(vulkan_logical_device, VK_OBJECT_TYPE_SAMPLER,
                              std::format("{} default sampler", name),
                              m_image_info->m_sampler);
}

template <typename base_texture>
bool texture<base_texture>::fill_sampler_create_info_from(
    const texture_asset& asset, VkSamplerCreateInfo& out_sampler_create_info) {
  ReturnUnless(asset.m_sampler.has_value(), false);

  auto& model_sampler = asset.m_sampler.value();

  out_sampler_create_info.addressModeU =
      s_address_mode[model_sampler.m_address_mode_u];
  out_sampler_create_info.addressModeV =
      s_address_mode[model_sampler.m_address_mode_v];

  out_sampler_create_info.magFilter = s_filters[model_sampler.m_mag_filter];
  out_sampler_create_info.minFilter = s_filters[model_sampler.m_min_filter];
  out_sampler_create_info.mipmapMode = s_mip_map[model_sampler.m_mipmap_mode];

  out_sampler_create_info.borderColor =
      s_border_colour_map[model_sampler.m_border_colour];

  return true;
}

template <typename base_texture>
void texture<base_texture>::create_image_view(
    const std::string& name,
    VkFormat image_format) {  // Create a default image view
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
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
  image_view_create_info.image = m_image_info->m_image;

  VK_CHECK_RESULT(vkCreateImageView(vulkan_logical_device,
                                    &image_view_create_info, nullptr,
                                    &(m_image_info->m_image_view)));

  base_texture::m_descriptor.imageView = m_image_info->m_image_view;

  set_debug_utils_object_name(vulkan_logical_device, VK_OBJECT_TYPE_IMAGE_VIEW,
                              std::format("{} default image view", name),
                              m_image_info->m_image_view);
}

template <typename base_texture>
void texture<base_texture>::allocate_image(const std::string& name,
                                           VkFormat image_format,
                                           std::uint32_t width,
                                           std::uint32_t height) {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vulkan_logical_device = device.get_vulkan_logical_device();
  auto& allocator = vulkan_context.mutable_resource_allocator();

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

  VkImageCreateInfo image_create_info = {};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = VK_IMAGE_TYPE_2D;
  image_create_info.format = image_format;
  image_create_info.extent.width = width;
  image_create_info.extent.height = height;
  image_create_info.extent.depth = 1;
  image_create_info.mipLevels = m_mip_levels;
  image_create_info.arrayLayers = 1;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
  image_create_info.usage = base_texture::s_usage |
                            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                            VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  m_image_info->m_memory_alloc =
      allocator.allocate_image(image_create_info, memoryUsage,
                               m_image_info->m_image, &m_gpu_allocation_size);
  set_debug_utils_object_name(vulkan_logical_device, VK_OBJECT_TYPE_IMAGE,
                              std::format("{} default image view", name),
                              m_image_info->m_image);
}

template <typename base_texture>
void texture<base_texture>::bind_texture(VkImageLayout target_layout) {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& command_pool = vulkan_context.mutable_command_pool() ;
  VkCommandBuffer command_buffer =
      command_pool.get_current_compute_command_buffer();

  transit_image_layout(command_buffer, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
                       target_layout);

  command_pool.flush_compute_command_buffer();
}

template <typename base_texture>
void texture<base_texture>::bind_texture_data(const texture_asset& asset,
                                              VkImageLayout target_layout) {
  // TODO:: handle non-existing texture
  const auto& texture_data = asset.m_texture_data;
  ReturnIf(texture_data.is_empty());

  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& command_pool = vulkan_context.mutable_command_pool() ;
  auto& allocator = vulkan_context.mutable_resource_allocator();

  VkMemoryAllocateInfo memAllocInfo{};
  memset(&memAllocInfo, 0, sizeof(VkMemoryAllocateInfo));

  memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

  // Create staging buffer
  VkBufferCreateInfo buffer_create_info{};
  memset(&buffer_create_info, 0, sizeof(VkBufferCreateInfo));

  buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_create_info.size = texture_data.size();
  buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  VkBuffer staging_buffer;
  VmaAllocation staging_buffer_allocation = allocator.allocate_buffer(
      buffer_create_info, VMA_MEMORY_USAGE_CPU_TO_GPU, staging_buffer);

  // Copy data to staging buffer
  texture_data.copy_to(staging_buffer_allocation);
  allocator.unmap_memory(staging_buffer_allocation);

  VkCommandBuffer command_buffer =
      command_pool.get_current_compute_command_buffer();

  VkBufferImageCopy buffer_copy_region = {};
  buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  buffer_copy_region.imageSubresource.mipLevel = 0;
  buffer_copy_region.imageSubresource.baseArrayLayer = 0;
  buffer_copy_region.imageSubresource.layerCount = m_mip_levels;
  buffer_copy_region.imageExtent.width = asset.m_width;
  buffer_copy_region.imageExtent.height = asset.m_height;
  buffer_copy_region.imageExtent.depth = 1;
  buffer_copy_region.bufferOffset = 0;

  transit_image_layout(command_buffer, VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
                       VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  // Copy mip levels from staging buffer
  vkCmdCopyBufferToImage(command_buffer, staging_buffer, m_image_info->m_image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                         &buffer_copy_region);

  transit_image_layout(command_buffer,
                       VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       target_layout);

  command_pool.flush_compute_command_buffer();

  allocator.destroy_buffer(staging_buffer, staging_buffer_allocation);
}

template <typename base_texture>
void texture<base_texture>::transit_image_layout(
    VkCommandBuffer& command_buffer, VkImageLayout old_layout,
    VkImageLayout new_layout) {
  // Image memory barriers for the texture image

  // The sub resource range describes the regions of the image that will be
  // transitioned using the memory barriers below
  VkImageSubresourceRange subresource_range = {};
  memset(&subresource_range, 0, sizeof(VkImageSubresourceRange));
  // Image only contains color data
  subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  // Start at first mip level
  subresource_range.baseMipLevel = 0;
  subresource_range.levelCount = m_mip_levels;
  subresource_range.layerCount = 1;

  // Transition the texture image layout to transfer target, so we can safely
  // copy our buffer data to it.
  VkImageMemoryBarrier image_memory_barrier{};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = m_image_info->m_image;
  image_memory_barrier.subresourceRange = subresource_range;
  image_memory_barrier.srcAccessMask =
      access_flags_for_image_layout(old_layout);
  image_memory_barrier.dstAccessMask =
      access_flags_for_image_layout(new_layout);
  image_memory_barrier.oldLayout = old_layout;
  image_memory_barrier.newLayout = new_layout;

  // Insert a memory dependency at the proper pipeline stages that will execute
  // the image layout transition Source pipeline stage is host write/read
  // exection (VK_PIPELINE_STAGE_HOST_BIT) Destination pipeline stage is copy
  // command exection (VK_PIPELINE_STAGE_TRANSFER_BIT)
  vkCmdPipelineBarrier(command_buffer, pipeline_stage_for_layout(old_layout),
                       pipeline_stage_for_layout(new_layout), 0, 0, nullptr, 0,
                       nullptr, 1, &image_memory_barrier);
}

template <typename base_texture>
std::string texture<base_texture>::generate_next_texture_name() {
  return "texture" + std::to_string(texture_counter++);
}

template class texture<vulkan::shader_resource::instance::sampled_image>;

template class texture<vulkan::shader_resource::instance::storage_image>;
}  // namespace wunder::vulkan