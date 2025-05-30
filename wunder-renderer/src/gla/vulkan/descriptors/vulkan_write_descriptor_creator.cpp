#include "gla/vulkan/descriptors/vulkan_write_descriptor_creator.h"

#include <cstring>

#include "core/wunder_macros.h"

namespace wunder::vulkan {

VkWriteDescriptorSet write_descriptor_creator::operator()(
    const std::vector<VkDescriptorImageInfo>& resource) {
  VkWriteDescriptorSet result = {};

  result.descriptorCount = static_cast<uint32_t>(resource.size());
  result.pImageInfo = resource.data();

  return result;
}

VkWriteDescriptorSet write_descriptor_creator::operator()(
    const std::vector<VkDescriptorBufferInfo>& resource) {
  VkWriteDescriptorSet result = {};

  result.descriptorCount = static_cast<uint32_t>(resource.size());
  result.pBufferInfo = resource.data();

  return result;
}

VkWriteDescriptorSet write_descriptor_creator::operator()(
    std::vector<VkBufferView>& resource) {
  VkWriteDescriptorSet result = {};

  result.descriptorCount = static_cast<uint32_t>(resource.size());
  result.pTexelBufferView = resource.data();

  return result;
}

VkWriteDescriptorSet write_descriptor_creator::operator()(
    std::vector<VkAccelerationStructureKHR>& resources) {
  VkWriteDescriptorSet result = {};
  ReturnIf(resources.empty(), result);

  // TODO:: this is a workaround. However currently it has no impacts
  //  problematic sections are multi-thread access and having multiple
  //  descriptors of this type, placed in separate bindings
  static VkWriteDescriptorSetAccelerationStructureKHR
      descriptor_set_acceleration_structure = {};

  descriptor_set_acceleration_structure.sType =
      VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
  descriptor_set_acceleration_structure.accelerationStructureCount =
      static_cast<uint32_t>(resources.size());
  descriptor_set_acceleration_structure.pAccelerationStructures =
      resources.data();

  result.descriptorCount =
      descriptor_set_acceleration_structure.accelerationStructureCount;
  result.pNext = &descriptor_set_acceleration_structure;
  return result;
}
}  // namespace wunder::vulkan