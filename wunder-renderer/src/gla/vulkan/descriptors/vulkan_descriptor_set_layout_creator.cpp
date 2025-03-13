#include "gla/vulkan/descriptors/vulkan_descriptor_set_layout_creator.h"

#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"

namespace wunder::vulkan {

descriptor_set_layout_creator::descriptor_set_layout_creator(
    descriptor_set_manager& descriptor_set_manager)
    : m_descriptor_set_manager(descriptor_set_manager) {}

VkDescriptorSetLayoutBinding descriptor_set_layout_creator::operator()(
    const shader_resource::declaration::uniform_buffer& resource) {
  VkDescriptorSetLayoutBinding layout_binding;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layout_binding.descriptorCount =
      try_retrieve_binding_count(resource);  // TODO
  layout_binding.stageFlags = VK_SHADER_STAGE_ALL;
  layout_binding.pImmutableSamplers = nullptr;
  layout_binding.binding = resource.m_binding;
  return layout_binding;
}

VkDescriptorSetLayoutBinding descriptor_set_layout_creator::operator()(
    const shader_resource::declaration::storage_buffers& resource) {
  VkDescriptorSetLayoutBinding layout_binding;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
  layout_binding.descriptorCount = try_retrieve_binding_count(resource);
  layout_binding.stageFlags = VK_SHADER_STAGE_ALL;
  layout_binding.pImmutableSamplers = nullptr;
  layout_binding.binding = resource.m_binding;
  return layout_binding;
}

VkDescriptorSetLayoutBinding descriptor_set_layout_creator::operator()(
    const shader_resource::declaration::sampled_images& resource) {
  VkDescriptorSetLayoutBinding layout_binding;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  layout_binding.descriptorCount = try_retrieve_binding_count(resource);
  layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
  layout_binding.pImmutableSamplers = nullptr;
  layout_binding.binding = resource.m_binding;
  return layout_binding;
}

VkDescriptorSetLayoutBinding descriptor_set_layout_creator::operator()(
    const shader_resource::declaration::separate_images& resource) {
  VkDescriptorSetLayoutBinding layout_binding;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
  layout_binding.descriptorCount = try_retrieve_binding_count(resource);
  layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
  layout_binding.pImmutableSamplers = nullptr;
  layout_binding.binding = resource.m_binding;
  return layout_binding;
}

VkDescriptorSetLayoutBinding descriptor_set_layout_creator::operator()(
    const shader_resource::declaration::separate_samplers& resource) {
  VkDescriptorSetLayoutBinding layout_binding;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
  layout_binding.descriptorCount = try_retrieve_binding_count(resource);
  layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
  layout_binding.pImmutableSamplers = nullptr;
  layout_binding.binding = resource.m_binding;
  return layout_binding;
}

VkDescriptorSetLayoutBinding descriptor_set_layout_creator::operator()(
    const shader_resource::declaration::storage_images& resource) {
  VkDescriptorSetLayoutBinding layout_binding;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
  layout_binding.descriptorCount = try_retrieve_binding_count(resource);
  layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
  layout_binding.pImmutableSamplers = nullptr;
  layout_binding.binding = resource.m_binding;
  return layout_binding;
}

VkDescriptorSetLayoutBinding descriptor_set_layout_creator::operator()(
    const shader_resource::declaration::acceleration_structures& resource) {
  VkDescriptorSetLayoutBinding layout_binding;
  layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
  layout_binding.descriptorCount = try_retrieve_binding_count(resource);
  layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
  layout_binding.pImmutableSamplers = nullptr;
  layout_binding.binding = resource.m_binding;
  return layout_binding;
}

uint32_t descriptor_set_layout_creator::try_retrieve_binding_count(
    const shader_resource::declaration::base& resource) {
  auto maybe_binding = m_descriptor_set_manager.find_resource_binding(
      resource.m_set, resource.m_binding);
  uint32_t binding_count = 1;
  if (maybe_binding) {
    binding_count = maybe_binding->get().size();
  }

  return binding_count;
}

}  // namespace wunder::vulkan
