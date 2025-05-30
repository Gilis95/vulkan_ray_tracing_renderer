#include "gla/vulkan/vulkan_memory_allocator.h"

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
static std::map<VmaAllocation, AllocInfo> k_allocation_map;

memory_allocator::memory_allocator(std::string tag) : m_tag(std::move(tag)) {}

memory_allocator::~memory_allocator() {
  char* statsString = nullptr;
  vmaBuildStatsString(m_resource_allocator, &statsString,
                      VK_TRUE);  // VK_TRUE = detailed
  WUNDER_WARN("{0}\n", statsString);
  vmaFreeStatsString(m_resource_allocator, statsString);

  vmaDestroyAllocator(m_resource_allocator);
}

void memory_allocator::initialize() {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.mutable_physical_device();
  auto& device = vulkan_context.mutable_device();
  auto& vulkan = vulkan_context.mutable_vulkan();

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
    VkBufferCreateInfo buffer_create_info, VmaMemoryUsage usage,
    VkBuffer& out_buffer) {
  AssertReturnIf(buffer_create_info.size <= 0, VK_NULL_HANDLE);

  VmaAllocationCreateInfo alloc_create_info = {};
  alloc_create_info.usage = usage;

  VmaAllocation allocation;
  vmaCreateBuffer(m_resource_allocator, &buffer_create_info, &alloc_create_info,
                  &out_buffer, &allocation, nullptr);
  if (allocation == nullptr) {
    WUNDER_ERROR_TAG(m_tag, "Failed to allocate GPU buffer!");
    WUNDER_ERROR_TAG(m_tag, "  Requested size: {}",
                     string::utils::bytes_to_string(buffer_create_info.size));
  }

  VmaAllocationInfo allocation_info{};
  vmaGetAllocationInfo(m_resource_allocator, allocation, &allocation_info);
  WUNDER_WARN_TAG(m_tag, "allocating buffer; size = {0}",
                  string::utils::bytes_to_string(allocation_info.size));

  return allocation;
}

VmaAllocation memory_allocator::allocate_image(
    VkImageCreateInfo image_create_info, VmaMemoryUsage usage,
    VkImage& outImage, VkDeviceSize* allocatedSize) {
  VmaAllocationCreateInfo alloc_create_info = {};
  alloc_create_info.usage = usage;

  VmaAllocation allocation;
  vmaCreateImage(m_resource_allocator, &image_create_info, &alloc_create_info,
                 &outImage, &allocation, nullptr);
  if (allocation == nullptr) {
    WUNDER_ERROR_TAG(m_tag, "Failed to allocate GPU image!");
    WUNDER_ERROR_TAG(
        m_tag, "  Requested size: {}x{}x{}", image_create_info.extent.width,
        image_create_info.extent.height, image_create_info.extent.depth);
    WUNDER_ERROR_TAG(m_tag, "  Mips: {}", image_create_info.mipLevels);
    WUNDER_ERROR_TAG(m_tag, "  Layers: {}", image_create_info.arrayLayers);
  }

  VmaAllocationInfo allocInfo;
  vmaGetAllocationInfo(m_resource_allocator, allocation, &allocInfo);
  if (allocatedSize) {
    *allocatedSize = allocInfo.size;
  }

  WUNDER_WARN_TAG(m_tag, "Allocating image; size = {}",
                  string::utils::bytes_to_string(allocInfo.size));

  return allocation;
}

void memory_allocator::free(VmaAllocation allocation) {
  vmaFreeMemory(m_resource_allocator, allocation);
}

void memory_allocator::destroy_image(VkImage image, VmaAllocation allocation) {
  AssertReturnUnless(image);
  AssertReturnUnless(allocation);

  VmaAllocationInfo allocInfo;
  vmaGetAllocationInfo(m_resource_allocator, allocation, &allocInfo);

  WUNDER_WARN_TAG(m_tag, "Deallocating image; size = {}",
                  string::utils::bytes_to_string(allocInfo.size));

  vmaDestroyImage(m_resource_allocator, image, allocation);
}

void memory_allocator::destroy_buffer(VkBuffer buffer,
                                      VmaAllocation allocation) {
  AssertReturnUnless(buffer);
  AssertReturnUnless(allocation);

  VmaAllocationInfo allocInfo;
  vmaGetAllocationInfo(m_resource_allocator, allocation, &allocInfo);

  WUNDER_WARN_TAG(m_tag, "Deallocating buffer; size = {}",
                  string::utils::bytes_to_string(allocInfo.size));

  vmaDestroyBuffer(m_resource_allocator, buffer, allocation);
}

void memory_allocator::unmap_memory(VmaAllocation allocation) {
  vmaUnmapMemory(m_resource_allocator, allocation);
}

void memory_allocator::dump_stats() {
  VmaTotalStatistics stats;
  vmaCalculateStatistics(m_resource_allocator, &stats);

  WUNDER_WARN_TAG(m_tag, "-----------------------------------");
  for (auto& b : stats.memoryHeap) {
    WUNDER_WARN_TAG(
        m_tag, "VmaBudget.allocationBytes = {0}",
        string::utils::bytes_to_string(b.statistics.allocationBytes));
    WUNDER_WARN_TAG(m_tag, "VmaBudget.blockBytes = {0}",
                    string::utils::bytes_to_string(b.statistics.blockBytes));
    WUNDER_WARN_TAG(m_tag, "VmaBudget.usage = {0}",
                    string::utils::bytes_to_string(b.allocationSizeMax));
    WUNDER_WARN_TAG(m_tag, "VmaBudget.budget = {0}",
                    string::utils::bytes_to_string(b.unusedRangeSizeMax));
  }
  WUNDER_WARN_TAG(m_tag, "-----------------------------------");
}

VmaAllocator& memory_allocator::get_vma_allocator() {
  return m_resource_allocator;
}

}  // namespace wunder::vulkan