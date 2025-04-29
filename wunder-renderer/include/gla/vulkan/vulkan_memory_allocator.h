#ifndef WUNDER_VULKAN_MEMORY_ALLOCATOR_H
#define WUNDER_VULKAN_MEMORY_ALLOCATOR_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include <string>

#include "core/non_copyable.h"

namespace wunder::vulkan {
class memory_allocator : public non_copyable{
 public:
  memory_allocator() = default;
  explicit memory_allocator(std::string tag);
  ~memory_allocator();

 public:
  void initialize();

 public:
  // void Allocate(VkMemoryRequirements requirements, VkDeviceMemory* dest,
  // VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VmaAllocation allocate_buffer(VkBufferCreateInfo buffer_create_info,
                                VmaMemoryUsage usage, VkBuffer& out_buffer);
  VmaAllocation allocate_image(VkImageCreateInfo image_create_info,
                               VmaMemoryUsage usage, VkImage& outImage,
                               VkDeviceSize* allocatedSize = nullptr);
  void free(VmaAllocation allocation);
  void destroy_image(VkImage image, VmaAllocation allocation);
  void destroy_buffer(VkBuffer buffer, VmaAllocation allocation);

  template <typename T>
  T* map_memory(VmaAllocation allocation) {
    T* mappedMemory;
    vmaMapMemory(memory_allocator::get_vma_allocator(), allocation,
                 reinterpret_cast<void**>(&mappedMemory));
    return mappedMemory;
  }

  void unmap_memory(VmaAllocation allocation);
  void dump_stats();

  VmaAllocator& get_vma_allocator();

 private:
  std::string m_tag;
  VmaAllocator m_resource_allocator;
};

}  // namespace wunder

#endif  // WUNDER_VULKAN_MEMORY_ALLOCATOR_H
