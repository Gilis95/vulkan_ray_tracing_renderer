#ifndef WUNDER_VULKAN_MEMORY_ALLOCATOR_H
#define WUNDER_VULKAN_MEMORY_ALLOCATOR_H

#include <glad/vulkan.h>
#include <vk_mem_alloc.h>

#include <string>

namespace wunder {
class vulkan_memory_allocator {
 public:
  vulkan_memory_allocator() = default;
  vulkan_memory_allocator(std::string tag);
  ~vulkan_memory_allocator();

 public:
  void initialize();

 public:
  // void Allocate(VkMemoryRequirements requirements, VkDeviceMemory* dest,
  // VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  VmaAllocation allocate_buffer(VkBufferCreateInfo bufferCreateInfo,
                                VmaMemoryUsage usage, VkBuffer& outBuffer);
  VmaAllocation allocate_image(VkImageCreateInfo imageCreateInfo,
                               VmaMemoryUsage usage, VkImage& outImage,
                               VkDeviceSize* allocatedSize = nullptr);
  void free(VmaAllocation allocation);
  void destroy_image(VkImage image, VmaAllocation allocation);
  void destroy_buffer(VkBuffer buffer, VmaAllocation allocation);

  template <typename T>
  T* map_memory(VmaAllocation allocation) {
    T* mappedMemory;
    vmaMapMemory(vulkan_memory_allocator::get_vma_allocator(), allocation,
                 (void**)&mappedMemory);
    return mappedMemory;
  }

  void unmap_memory(VmaAllocation allocation);
  void dump_stats();

  VmaAllocator& get_vma_allocator();

 private:
  std::string m_Tag;
  VmaAllocator m_resource_allocator;
};

}  // namespace wunder

#endif  // WUNDER_VULKAN_MEMORY_ALLOCATOR_H
