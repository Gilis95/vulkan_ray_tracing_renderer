#ifndef VULKAN_WRITE_DESCRIPTOR_CREATOR_H
#define VULKAN_WRITE_DESCRIPTOR_CREATOR_H

#include <glad/vulkan.h>
#include <vector>

namespace wunder::vulkan {

class write_descriptor_creator {
 public:
  VkWriteDescriptorSet operator()(
      const std::vector<VkDescriptorImageInfo>& resource);
  VkWriteDescriptorSet operator()(
      const std::vector<VkDescriptorBufferInfo>& resource);
  VkWriteDescriptorSet operator()(std::vector<VkBufferView>& resource);
  VkWriteDescriptorSet operator()(
      std::vector<VkAccelerationStructureKHR>& resources);
};
}

#endif //VULKAN_WRITE_DESCRIPTOR_CREATOR_H
