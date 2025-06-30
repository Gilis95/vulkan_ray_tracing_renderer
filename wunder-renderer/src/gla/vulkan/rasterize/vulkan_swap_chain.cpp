#include "gla/vulkan/rasterize/vulkan_swap_chain.h"

#include <oneapi/tbb/task_group.h>

#include "gla/vulkan/rasterize/vulkan_render_pass.h"
#include "gla/vulkan/vulkan.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "window/window_factory.h"

namespace wunder::vulkan {
swap_chain::queue_element::queue_element() = default;

swap_chain::queue_element::queue_element(queue_element&& other)
    : m_image(other.m_image),
      m_image_view(other.m_image_view),
      m_command_buffer(other.m_command_buffer),
      m_framebuffer(other.m_framebuffer),
      m_barrier(other.m_barrier),
      m_fence(other.m_fence),
      m_semaphore_entry(std::move(other.m_semaphore_entry))

{
  other.m_image = VK_NULL_HANDLE;
  other.m_image_view = VK_NULL_HANDLE;
  other.m_command_buffer = VK_NULL_HANDLE;
  other.m_framebuffer = VK_NULL_HANDLE;
  other.m_fence = VK_NULL_HANDLE;
  other.m_semaphore_entry.read_semaphore = VK_NULL_HANDLE;
  other.m_semaphore_entry.written_semaphore = VK_NULL_HANDLE;
};

swap_chain::queue_element::~queue_element() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  if (m_image_view != VK_NULL_HANDLE) {
    vkDestroyImageView(vk_device, m_image_view, VK_NULL_HANDLE);
  }

  if (m_semaphore_entry.read_semaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(vk_device, m_semaphore_entry.read_semaphore,
                       VK_NULL_HANDLE);
  }

  if (m_semaphore_entry.written_semaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(vk_device, m_semaphore_entry.written_semaphore,
                       VK_NULL_HANDLE);
  }

  if (m_framebuffer != VK_NULL_HANDLE) {
    vkDestroyFramebuffer(vk_device, m_framebuffer, VK_NULL_HANDLE);
  }

  if (m_fence != VK_NULL_HANDLE) {
    vkDestroyFence(vk_device, m_fence, VK_NULL_HANDLE);
  }

  // if (m_image != VK_NULL_HANDLE) {
  //   vkDestroyImage(vk_device, m_image, VK_NULL_HANDLE);
  // }
}

swap_chain::swap_chain(std::uint32_t width, std::uint32_t height)
    : m_vsync_enabled(false),
      m_width(width),
      m_height(height),
      m_current_queue_element{},
      m_queue_elements{},
      m_render_pass(),
      m_surface(VK_NULL_HANDLE),
      m_swap_chain(VK_NULL_HANDLE),
      m_command_pool(VK_NULL_HANDLE),
      m_depth_image(VK_NULL_HANDLE),
      m_depth_memory(VK_NULL_HANDLE),
      m_depth_view(VK_NULL_HANDLE),
      m_colour_format{VK_FORMAT_B8G8R8A8_UNORM},
      m_color_space{VK_COLOR_SPACE_SRGB_NONLINEAR_KHR} {}

swap_chain::~swap_chain() = default;

void swap_chain::resize(uint32_t width, uint32_t height) {
  wait_idle();

  shutdown();

  m_width = width;
  m_height = height;

  init();
  wait_idle();
}

void swap_chain::init() {
  initialize_swap_chain();
  initialize_render_pass();
  initialize_depth_buffer();
  initialize_queue_elements();
}

void swap_chain::shutdown() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  wait_idle();

  if (m_swap_chain) {
    vkDestroySwapchainKHR(vk_device, m_swap_chain, nullptr);
    m_swap_chain = VK_NULL_HANDLE;
  }

  if (m_command_pool != VK_NULL_HANDLE) {
    vkDestroyCommandPool(vk_device, m_command_pool, VK_NULL_HANDLE);
  }

  if (m_depth_image != VK_NULL_HANDLE) {
    vkDestroyImage(vk_device, m_depth_image, VK_NULL_HANDLE);
  }

  if (m_depth_memory != VK_NULL_HANDLE) {
    vkDestroyImageView(vk_device, m_depth_view, VK_NULL_HANDLE);
  }

  if (m_depth_memory != VK_NULL_HANDLE) {
    vkFreeMemory(vk_device, m_depth_memory, nullptr);
  }

  m_queue_elements.clear();

  if (m_render_pass) {
    m_render_pass.reset();
  }

  if (m_surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(vulkan_context.mutable_vulkan().get_instance(),
                        m_surface, VK_NULL_HANDLE);
  }
}

std::optional<std::uint32_t> swap_chain::acquire() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  for (int i = 0; i < 2; i++) {
    auto next_image_idx = (m_current_queue_element) % m_queue_elements.size();
    VkSemaphore semaphore =
        m_queue_elements[next_image_idx].m_semaphore_entry.read_semaphore;

    VkResult result;
    // other option would be having an amount of semaphores with 1 greater than
    // a swap chain elements
    wait_element_to_rendered(next_image_idx);
    result = vkAcquireNextImageKHR(vk_device, m_swap_chain, 10000, semaphore,
                                   (VkFence)VK_NULL_HANDLE,
                                   &m_current_queue_element);
    wait_element_to_rendered(m_current_queue_element);

    if (result == VK_SUCCESS) {
      return m_current_queue_element;
    }

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
      resize(m_width, m_height);
    }
  }

  return std::nullopt;
}

void swap_chain::begin_command_buffer() {
  auto command_buffer =
      m_queue_elements[m_current_queue_element].m_command_buffer;

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(command_buffer, &beginInfo);
}

VkCommandBuffer swap_chain::get_current_command_buffer() {
  return m_queue_elements[m_current_queue_element].m_command_buffer;
}

void swap_chain::flush_current_command_buffer() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();
  auto device_queue = device.get_graphics_queue();
  auto& queue_element = m_queue_elements[m_current_queue_element];

  vkEndCommandBuffer(queue_element.m_command_buffer);

  vkResetFences(vk_device, 1, &queue_element.m_fence);

  bool m_use_nv_link = false;
  // In case of using NVLINK
  const uint32_t deviceMask = m_use_nv_link ? 0b0000'0011 : 0b0000'0001;
  const std::array<uint32_t, 2> deviceIndex = {0, 1};

  VkDeviceGroupSubmitInfo deviceGroupSubmitInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_SUBMIT_INFO_KHR,
      .pNext = VK_NULL_HANDLE,
      .waitSemaphoreCount = 1,
      .pWaitSemaphoreDeviceIndices = deviceIndex.data(),
      .commandBufferCount = 1,
      .pCommandBufferDeviceMasks = &deviceMask,
      .signalSemaphoreCount = m_use_nv_link ? 2u : 1u,
      .pSignalSemaphoreDeviceIndices = deviceIndex.data(),
  };

  // Pipeline stage at which the queue submission will wait (via
  // pWaitSemaphores)
  const VkPipelineStageFlags waitStageMask =
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  // The submit info structure specifies a command buffer queue submission batch
  VkSubmitInfo submitInfo{
      .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
      .pNext = &deviceGroupSubmitInfo,
      .waitSemaphoreCount = 1,  // One wait semaphore
      .pWaitSemaphores = &queue_element.m_semaphore_entry.read_semaphore,
      .pWaitDstStageMask = &waitStageMask,
      .commandBufferCount = 1,  // One command buffer
      .pCommandBuffers = &queue_element.m_command_buffer,
      .signalSemaphoreCount = 1,  // One signal semaphore
      .pSignalSemaphores = &queue_element.m_semaphore_entry.written_semaphore,
  };

  // Submit to the graphics queue passing a wait fence
  vkQueueSubmit(device_queue, 1, &submitInfo, queue_element.m_fence);

  VkPresentInfoKHR present_info{
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = VK_NULL_HANDLE,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &queue_element.m_semaphore_entry.written_semaphore,
      .swapchainCount = 1,
      .pSwapchains = &m_swap_chain,
      .pImageIndices = &m_current_queue_element,
      .pResults = VK_NULL_HANDLE,
  };

  VK_CHECK_RESULT(vkQueuePresentKHR(device_queue, &present_info));

  ++m_current_queue_element;
}

void swap_chain::begin_render_pass() const {
  m_render_pass->begin(m_queue_elements[m_current_queue_element].m_framebuffer,
                       {.width = m_width, .height = m_height});
}

void swap_chain::end_render_pass() const { m_render_pass->end(); }

render_pass& swap_chain::mutable_render_pass() { return *m_render_pass; }

void swap_chain::initialize_swap_chain() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.mutable_physical_device();
  auto& device = vulkan_context.mutable_device();
  auto vk_physical_device = physical_device.get_vulkan_physical_device();
  auto vk_device = device.get_vulkan_logical_device();

  VkSwapchainKHR old_swap_chain = m_swap_chain;

  m_surface = window_factory::instance().get_window().create_vulkan_surface();

  uint32_t surface_format_count;
  VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(
      vk_physical_device, m_surface, &surface_format_count, nullptr));

  std::vector<VkSurfaceFormatKHR> surface_formats(surface_format_count);
  VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(
      vk_physical_device, m_surface, &surface_format_count,
      surface_formats.data()));

  m_color_space = surface_formats[0].colorSpace;
  m_colour_format = surface_formats[0].format;

  // Get physical device surface properties and formats
  VkSurfaceCapabilitiesKHR surface_capabilities;
  VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
      vk_physical_device, m_surface, &surface_capabilities));

  // Get available present modes
  uint32_t present_mode_count;
  VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(
      vk_physical_device, m_surface, &present_mode_count, nullptr));
  AssertReturnUnless(present_mode_count > 0);

  std::vector<VkPresentModeKHR> presentModes(present_mode_count);
  VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(
      vk_physical_device, m_surface, &present_mode_count, presentModes.data()));

  VkExtent2D swapchain_extent = {};
  // If width (and height) equals the special value 0xFFFFFFFF, the size of the
  // surface will be set by the swapchain
  if (surface_capabilities.currentExtent.width == (uint32_t)-1) {
    // If the surface size is undefined, the size is set to
    // the size of the images requested.
    swapchain_extent.width = m_width;
    swapchain_extent.height = m_height;
  } else {
    // If the surface size is defined, the swap chain size must match
    swapchain_extent = surface_capabilities.currentExtent;
    m_width = surface_capabilities.currentExtent.width;
    m_height = surface_capabilities.currentExtent.height;
  }

  if (m_width == 0 || m_height == 0) return;

  // Select a present mode for the swapchain

  // The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
  // This mode waits for the vertical blank ("v-sync")
  VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;

  // If v-sync is not requested, try to find a mailbox mode
  // It's the lowest latency non-tearing present mode available
  if (!m_vsync_enabled) {
    for (size_t i = 0; i < present_mode_count; i++) {
      if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
        swapchain_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
        break;
      }
      if ((swapchain_present_mode != VK_PRESENT_MODE_MAILBOX_KHR) &&
          (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
        swapchain_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
      }
    }
  }

  // Determine the number of images
  uint32_t desiredNumberOfSwapchainImages =
      surface_capabilities.minImageCount + 1;
  if ((surface_capabilities.maxImageCount > 0) &&
      (desiredNumberOfSwapchainImages > surface_capabilities.maxImageCount)) {
    desiredNumberOfSwapchainImages = surface_capabilities.maxImageCount;
  }

  // Find the transformation of the surface
  VkSurfaceTransformFlagsKHR preTransform;
  if (surface_capabilities.supportedTransforms &
      VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
    // We prefer a non-rotated transform
    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  } else {
    preTransform = surface_capabilities.currentTransform;
  }

  // Find a supported composite alpha format (not all devices support alpha
  // opaque)
  VkCompositeAlphaFlagBitsKHR compositeAlpha =
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  // Simply select the first composite alpha format available
  std::vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags = {
      VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
      VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
  };
  for (auto& compositeAlphaFlag : composite_alpha_flags) {
    if (surface_capabilities.supportedCompositeAlpha & compositeAlphaFlag) {
      compositeAlpha = compositeAlphaFlag;
      break;
    };
  }

  VkSwapchainCreateInfoKHR swapchain_create_info = {};
  swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_create_info.pNext = NULL;
  swapchain_create_info.surface = m_surface;
  swapchain_create_info.minImageCount = desiredNumberOfSwapchainImages;
  swapchain_create_info.imageFormat = m_colour_format;
  swapchain_create_info.imageColorSpace = m_color_space;
  swapchain_create_info.imageExtent = {swapchain_extent.width,
                                       swapchain_extent.height};
  swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                     VK_IMAGE_USAGE_STORAGE_BIT |
                                     VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  swapchain_create_info.preTransform =
      (VkSurfaceTransformFlagBitsKHR)preTransform;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_create_info.queueFamilyIndexCount = 0;
  swapchain_create_info.pQueueFamilyIndices = NULL;
  swapchain_create_info.presentMode = swapchain_present_mode;
  swapchain_create_info.oldSwapchain = old_swap_chain;
  // Setting clipped to VK_TRUE allows the implementation to discard rendering
  // outside of the surface area
  swapchain_create_info.clipped = VK_TRUE;
  swapchain_create_info.compositeAlpha = compositeAlpha;

  // Enable transfer source on swap chain images if supported
  if (surface_capabilities.supportedUsageFlags &
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  // Enable transfer destination on swap chain images if supported
  if (surface_capabilities.supportedUsageFlags &
      VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  VK_CHECK_RESULT(vkCreateSwapchainKHR(vk_device, &swapchain_create_info,
                                       nullptr, &m_swap_chain));

  ReturnUnless(old_swap_chain);
  vkDestroySwapchainKHR(vk_device, old_swap_chain, nullptr);
}

void swap_chain::initialize_render_pass() {  // Render Pass
  m_render_pass = make_unique<render_pass>(m_colour_format);
}

void swap_chain::initialize_depth_buffer() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.mutable_physical_device();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  if (m_depth_view) {
    vkDestroyImageView(vk_device, m_depth_view, nullptr);
  }

  if (m_depth_image) {
    vkDestroyImage(vk_device, m_depth_image, nullptr);
  }

  if (m_depth_memory) {
    vkFreeMemory(vk_device, m_depth_memory, nullptr);
  }

  // Depth information
  const VkImageAspectFlags aspect =
      VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
  VkImageCreateInfo depth_stencil_create_info{};
  depth_stencil_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  depth_stencil_create_info.imageType = VK_IMAGE_TYPE_2D;
  depth_stencil_create_info.extent = VkExtent3D{m_width, m_height, 1};
  depth_stencil_create_info.format = physical_device.get_depth_format();
  depth_stencil_create_info.mipLevels = 1;
  depth_stencil_create_info.arrayLayers = 1;
  depth_stencil_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_stencil_create_info.usage =
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  // Create the depth image
  vkCreateImage(vk_device, &depth_stencil_create_info, nullptr, &m_depth_image);

  set_debug_utils_object_name(vk_device, "depth image", m_depth_image);

  // Allocate the memory
  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(vk_device, m_depth_image, &memory_requirements);
  VkMemoryAllocateInfo mem_alloc_info{};
  mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  mem_alloc_info.allocationSize = memory_requirements.size;
  mem_alloc_info.memoryTypeIndex = physical_device.get_memory_type_index(
      memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  vkAllocateMemory(vk_device, &mem_alloc_info, nullptr, &m_depth_memory);
  set_debug_utils_object_name(vk_device, "depth image memory", m_depth_memory);

  // Bind image and memory
  vkBindImageMemory(vk_device, m_depth_image, m_depth_memory, 0);

  // Put barrier on top, Put barrier inside setup command buffer
  VkImageSubresourceRange subresource_range{};
  subresource_range.aspectMask = aspect;
  subresource_range.levelCount = 1;
  subresource_range.layerCount = 1;
  VkImageMemoryBarrier image_memory_barrier{};
  image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_memory_barrier.newLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
  image_memory_barrier.image = m_depth_image;
  image_memory_barrier.subresourceRange = subresource_range;
  image_memory_barrier.srcAccessMask = VkAccessFlags();
  image_memory_barrier.dstAccessMask =
      VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  const VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
  const VkPipelineStageFlags destStageMask =
      VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

  vkCmdPipelineBarrier(
      device.get_command_pool().get_current_graphics_command_buffer(),
      srcStageMask, destStageMask, VK_FALSE, 0, nullptr, 0, nullptr, 1,
      &image_memory_barrier);
  device.get_command_pool().flush_graphics_command_buffer();

  // Setting up the view
  VkImageViewCreateInfo depth_stencil_view{};
  depth_stencil_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  depth_stencil_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
  depth_stencil_view.format = physical_device.get_depth_format();
  depth_stencil_view.subresourceRange = subresource_range;
  depth_stencil_view.image = m_depth_image;
  vkCreateImageView(vk_device, &depth_stencil_view, nullptr, &m_depth_view);
}

void swap_chain::initialize_queue_elements() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  uint32_t image_count = 0;
  VK_CHECK_RESULT(
      vkGetSwapchainImagesKHR(vk_device, m_swap_chain, &image_count, nullptr));

  m_queue_elements.resize(image_count);

  create_image_for_each_queue_element();
  create_image_barrier_for_each_queue_element();
  create_command_buffer_for_each_queue_element();
  create_semaphores_for_each_queue_element();
  create_fence_for_each_queue_element();
  create_frame_buffer_for_each_queue_element();

  update_barriers();
}

void swap_chain::create_image_for_each_queue_element() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  std::uint32_t image_count = static_cast<uint32_t>(m_queue_elements.size());

  // Get the swap chain images
  std::vector<VkImage> images(image_count);
  VK_CHECK_RESULT(vkGetSwapchainImagesKHR(vk_device, m_swap_chain, &image_count,
                                          images.data()));

  // Get the swap chain buffers containing the image and imageview
  for (uint32_t i = 0; i < image_count; i++) {
    VkImage image =
        images[i];  // it's just a pointer, so we don't need a reference

    VkImageViewCreateInfo colorAttachmentView = {};
    colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    colorAttachmentView.pNext = VK_NULL_HANDLE;
    colorAttachmentView.format = m_colour_format;
    colorAttachmentView.image = image;
    colorAttachmentView.components = {
        VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A};
    colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorAttachmentView.subresourceRange.baseMipLevel = 0;
    colorAttachmentView.subresourceRange.levelCount = 1;
    colorAttachmentView.subresourceRange.baseArrayLayer = 0;
    colorAttachmentView.subresourceRange.layerCount = 1;
    colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorAttachmentView.flags = 0;

    auto& queue_element = m_queue_elements[i];

    queue_element.m_image = image;
    set_debug_utils_object_name(vk_device, std::format("swap chain image", i),
                                m_depth_image);

    VK_CHECK_RESULT(vkCreateImageView(vk_device, &colorAttachmentView, nullptr,
                                      &queue_element.m_image_view));
    set_debug_utils_object_name(vk_device,
                                std::format("Swapchain ImageView: {}", i),
                                queue_element.m_image_view);
  }
}

void swap_chain::create_image_barrier_for_each_queue_element() {
  for (auto& queue_element : m_queue_elements) {
    VkImageSubresourceRange range = {};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    VkImageMemoryBarrier& memory_barrier = queue_element.m_barrier;
    memset(&memory_barrier, 0, sizeof(VkImageMemoryBarrier));

    memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memory_barrier.dstAccessMask = 0;
    memory_barrier.srcAccessMask = 0;
    memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memory_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    memory_barrier.image = queue_element.m_image;
    memory_barrier.subresourceRange = range;
  }
}

void swap_chain::create_command_buffer_for_each_queue_element() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  VkCommandPoolCreateInfo poolCreateInfo{};
  poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  vkCreateCommandPool(vk_device, &poolCreateInfo, nullptr, &m_command_pool);

  VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
  commandBufferAllocateInfo.sType =
      VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocateInfo.commandBufferCount =
      static_cast<uint32_t>(m_queue_elements.size());
  commandBufferAllocateInfo.commandPool = m_command_pool;

  std::vector<VkCommandBuffer> command_buffers(m_queue_elements.size());

  VK_CHECK_RESULT(vkAllocateCommandBuffers(
      vk_device, &commandBufferAllocateInfo, command_buffers.data()));

  for (uint32_t i = 0; i < m_queue_elements.size(); i++) {
    m_queue_elements[i].m_command_buffer = command_buffers[i];
  }
}

void swap_chain::create_semaphores_for_each_queue_element() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  for (size_t i = 0; i < m_queue_elements.size(); i++) {
    auto& queue_element = m_queue_elements[i];

    VK_CHECK_RESULT(
        vkCreateSemaphore(vk_device, &semaphoreCreateInfo, nullptr,
                          &queue_element.m_semaphore_entry.read_semaphore));
    set_debug_utils_object_name(
        vk_device, std::format("Swapchain Semaphore ImageAvailable {0}", i),
        queue_element.m_semaphore_entry.read_semaphore);

    VK_CHECK_RESULT(
        vkCreateSemaphore(vk_device, &semaphoreCreateInfo, nullptr,
                          &queue_element.m_semaphore_entry.written_semaphore));
    set_debug_utils_object_name(
        vk_device, std::format("Swapchain Semaphore RenderFinished {0}", i),
        queue_element.m_semaphore_entry.written_semaphore);
  }
}

void swap_chain::create_fence_for_each_queue_element() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  // Create Synchronization Primitives
  for (size_t i = 0; i < m_queue_elements.size(); i++) {
    auto& queue_element = m_queue_elements[i];

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(vk_device, &fenceCreateInfo, nullptr, &queue_element.m_fence);
  }
}

void swap_chain::create_frame_buffer_for_each_queue_element() {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  std::array<VkImageView, 2> attachments{};
  attachments[1] = m_depth_view;

  VkFramebufferCreateInfo frameBufferCreateInfo = {};
  frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  frameBufferCreateInfo.renderPass = m_render_pass->get_vulkan_render_pass();
  frameBufferCreateInfo.attachmentCount = 2;
  frameBufferCreateInfo.width = m_width;
  frameBufferCreateInfo.height = m_height;
  frameBufferCreateInfo.layers = 1;
  frameBufferCreateInfo.pAttachments = attachments.data();

  for (size_t i = 0; i < m_queue_elements.size(); i++) {
    auto& queue_element = m_queue_elements[i];
    attachments[0] = queue_element.m_image_view;

    VK_CHECK_RESULT(vkCreateFramebuffer(vk_device, &frameBufferCreateInfo,
                                        nullptr, &queue_element.m_framebuffer));
    set_debug_utils_object_name(
        vk_device,
        std::format("Swapchain framebuffer (Frame in flight: {})", i),
        queue_element.m_framebuffer);
  }
}

void swap_chain::update_barriers() const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  command_pool& pool = device.get_command_pool();
  auto command_buffer = pool.get_current_compute_command_buffer();

  for (auto& queue_element : m_queue_elements) {
    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &queue_element.m_barrier);
  }

  pool.flush_compute_command_buffer();
}

void swap_chain::wait_idle() const {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  VK_CHECK_RESULT(vkDeviceWaitIdle(vk_device));
}

void swap_chain::wait_element_to_rendered(size_t element_idx) {
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();
  auto vk_device = device.get_vulkan_logical_device();

  VkResult result;
  do {
    result =
        vkWaitForFences(vk_device, 1, &m_queue_elements[element_idx].m_fence,
                        VK_TRUE, 1'000'000);
  } while (result == VK_TIMEOUT);
}

}  // namespace wunder::vulkan