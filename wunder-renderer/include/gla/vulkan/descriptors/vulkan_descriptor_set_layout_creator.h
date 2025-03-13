#ifndef DESCRIPTOR_SET_LAYOUT_CREATOR_H
#define DESCRIPTOR_SET_LAYOUT_CREATOR_H

#include <glad/vulkan.h>

#include "gla/vulkan/vulkan_shader_types.h"

namespace wunder::vulkan {
class descriptor_set_manager;

class descriptor_set_layout_creator {
  public:
  descriptor_set_layout_creator(descriptor_set_manager& descriptor_set_manager);
 public:
  VkDescriptorSetLayoutBinding operator()(
      const shader_resource::declaration::uniform_buffer& resource);

  VkDescriptorSetLayoutBinding operator()(
      const shader_resource::declaration::storage_buffers& resource);

  VkDescriptorSetLayoutBinding operator()(
      const shader_resource::declaration::sampled_images& resource);

  VkDescriptorSetLayoutBinding operator()(
      const shader_resource::declaration::separate_images& resource);

  VkDescriptorSetLayoutBinding operator()(
      const shader_resource::declaration::separate_samplers& resource);

  VkDescriptorSetLayoutBinding operator()(
      const shader_resource::declaration::storage_images& resource);

  VkDescriptorSetLayoutBinding operator()(
      const shader_resource::declaration::acceleration_structures& resource);

 private:
  uint32_t try_retrieve_binding_count(
      const shader_resource::declaration::base& resource);

 private:
  descriptor_set_manager& m_descriptor_set_manager;
};
}  // namespace wunder::vulkan
#endif  // DESCRIPTOR_SET_LAYOUT_CREATOR_H
