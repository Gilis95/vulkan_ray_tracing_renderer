#include "gla/vulkan/vulkan_memory_allocator.h"

#define VMA_IMPLEMENTATION 1
#include <vk_mem_alloc.h>

#include <map>
#include <utility>

#include "core/string_utils.h"
#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_physical_device.h"

namespace wunder::vulkan {
enum class AllocationType : uint8_t { None = 0, Buffer = 1, Image = 2 };

struct AllocInfo {
  uint64_t AllocatedSize = 0;
  AllocationType Type = AllocationType::None;
};
static std::map<VmaAllocation, AllocInfo> s_AllocationMap;

memory_allocator::memory_allocator(std::string tag)
    : m_Tag(std::move(tag)) {}

memory_allocator::~memory_allocator() {
  vmaDestroyAllocator(m_resource_allocator);
}

void memory_allocator::initialize() {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.get_physical_device();
  auto& device = vulkan_context.get_device();
  auto& vulkan = vulkan_context.get_vulkan();

  VmaAllocatorCreateInfo vma_allocator_create_info;
  memset(&vma_allocator_create_info, 0, sizeof(VmaAllocatorCreateInfo));

  vma_allocator_create_info.instance = vulkan.get_instance();
  vma_allocator_create_info.physicalDevice =
      physical_device.get_vulkan_physical_device();
  vma_allocator_create_info.device = device.get_vulkan_logical_device();
  vma_allocator_create_info.vulkanApiVersion = vulkan.get_vulkan_version();
  vma_allocator_create_info.flags |=
      VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

  vmaCreateAllocator(&vma_allocator_create_info, &m_resource_allocator);
}

VmaAllocation memory_allocator::allocate_buffer(
    VkBufferCreateInfo bufferCreateInfo, VmaMemoryUsage usage,
    VkBuffer& outBuffer) {
  AssertReturnIf(bufferCreateInfo.size <= 0, VK_NULL_HANDLE);

  VmaAllocationCreateInfo allocCreateInfo = {};
  allocCreateInfo.usage = usage;

  VmaAllocation allocation;
  vmaCreateBuffer(m_resource_allocator, &bufferCreateInfo, &allocCreateInfo,
                  &outBuffer, &allocation, nullptr);
  if (allocation == nullptr) {
    WUNDER_ERROR_TAG("Renderer", "Failed to allocate GPU buffer!");
    WUNDER_ERROR_TAG("Renderer", "  Requested size: {}",
                     string::utils::bytes_to_string(bufferCreateInfo.size));
  }

  VmaAllocationInfo allocInfo{};
  vmaGetAllocationInfo(m_resource_allocator, allocation, &allocInfo);
  WUNDER_WARN_TAG(
      "Renderer",
      "vulkan_memory_allocator ({0}): allocating buffer; size = {1}", m_Tag,
      string::utils::bytes_to_string(allocInfo.size));

  return allocation;
}

VmaAllocation memory_allocator::allocate_image(
    VkImageCreateInfo imageCreateInfo, VmaMemoryUsage usage, VkImage& outImage,
    VkDeviceSize* allocatedSize) {
  VmaAllocationCreateInfo allocCreateInfo = {};
  allocCreateInfo.usage = usage;

  VmaAllocation allocation;
  vmaCreateImage(m_resource_allocator, &imageCreateInfo, &allocCreateInfo,
                 &outImage, &allocation, nullptr);
  if (allocation == nullptr) {
    WUNDER_ERROR_TAG("Renderer", "Failed to allocate GPU image!");
    WUNDER_ERROR_TAG(
        "Renderer", "  Requested size: {}x{}x{}", imageCreateInfo.extent.width,
        imageCreateInfo.extent.height, imageCreateInfo.extent.depth);
    WUNDER_ERROR_TAG("Renderer", "  Mips: {}", imageCreateInfo.mipLevels);
    WUNDER_ERROR_TAG("Renderer", "  Layers: {}", imageCreateInfo.arrayLayers);
  }

  VmaAllocationInfo allocInfo;
  vmaGetAllocationInfo(m_resource_allocator, allocation, &allocInfo);
  if (allocatedSize) *allocatedSize = allocInfo.size;
  WUNDER_TRACE_TAG(m_Tag, "Allocating image; size = {}",
                   string::utils::bytes_to_string(allocInfo.size));

  return allocation;
}

void memory_allocator::free(VmaAllocation allocation) {
  vmaFreeMemory(m_resource_allocator, allocation);
}

void memory_allocator::destroy_image(VkImage image,
                                            VmaAllocation allocation) {
  AssertReturnUnless(image);
  AssertReturnUnless(allocation);
  vmaDestroyImage(m_resource_allocator, image, allocation);
}

void memory_allocator::destroy_buffer(VkBuffer buffer,
                                             VmaAllocation allocation) {
  AssertReturnUnless(buffer);
  AssertReturnUnless(allocation);
  vmaDestroyBuffer(m_resource_allocator, buffer, allocation);
}

void memory_allocator::unmap_memory(VmaAllocation allocation) {
  vmaUnmapMemory(m_resource_allocator, allocation);
}

void memory_allocator::dump_stats() {

  VmaTotalStatistics stats;
  vmaCalculateStatistics(m_resource_allocator,&stats);

  WUNDER_WARN_TAG("Renderer", "-----------------------------------");
  for (auto& b : stats.memoryHeap) {
    WUNDER_WARN_TAG(
        "Renderer", "VmaBudget.allocationBytes = {0}",
        string::utils::bytes_to_string(b.statistics.allocationBytes));
    WUNDER_WARN_TAG("Renderer", "VmaBudget.blockBytes = {0}",
                    string::utils::bytes_to_string(b.statistics.blockBytes));
    WUNDER_WARN_TAG("Renderer", "VmaBudget.usage = {0}",
                    string::utils::bytes_to_string(b.allocationSizeMax));
    WUNDER_WARN_TAG("Renderer", "VmaBudget.budget = {0}",
                    string::utils::bytes_to_string(b.unusedRangeSizeMax));
  }
  WUNDER_WARN_TAG("Renderer", "-----------------------------------");
}

VmaAllocator& memory_allocator::get_vma_allocator() {
  return m_resource_allocator;
}

}  // namespace wunder