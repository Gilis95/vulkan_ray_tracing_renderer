//
// Created by christian on 8/12/24.
//

#ifndef WUNDER_VULKAN_TYPES_H
#define WUNDER_VULKAN_TYPES_H

#include <glad/vulkan.h>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

namespace wunder {
using vulkan_resource_identifier = std::string;

using vulkan_descriptor_set_identifier = std::uint32_t;
using vulkan_descriptor_set_bind_identifier = std::uint32_t;

struct vulkan_shader_resource_declaration_base {
 public:
  vulkan_descriptor_set_identifier m_set = 0;
  vulkan_descriptor_set_bind_identifier m_binding = 0;
  std::uint32_t Count = 0;
};

// resources.uniform_buffers
// ShaderResource::UniformBuffer uniformBuffer;
struct vulkan_uniform_buffer_resource_declaration
    : public vulkan_shader_resource_declaration_base {};

// resources.storage_buffers
// ShaderResource::StorageBuffer storageBuffer;
struct vulkan_storage_buffers_resource_declaration
    : public vulkan_shader_resource_declaration_base {};

// resources.sampled_images
// auto& imageSampler = shaderDescriptorSet.ImageSamplers[binding];
struct vulkan_sampled_images_resource_declaration
    : public vulkan_shader_resource_declaration_base {};

// resources.separate_images
// shaderDescriptorSet.SeparateTextures[binding];
struct vulkan_separate_images_resource_declaration
    : public vulkan_shader_resource_declaration_base {};

// resources.separate_samplers
// shaderDescriptorSet.SeparateSamplers[binding];
struct vulkan_separate_samplers_resource_declaration
    : public vulkan_shader_resource_declaration_base {};

// resources.storage_images
// shaderDescriptorSet.StorageImages[binding]
struct vulkan_storage_images_resource_declaration
    : public vulkan_shader_resource_declaration_base {};

struct vulkan_acceleration_structures_resource_declaration
    : public vulkan_shader_resource_declaration_base {};

using vulkan_shader_resource_declaration =
    std::variant<vulkan_uniform_buffer_resource_declaration,
                 vulkan_storage_buffers_resource_declaration,
                 vulkan_sampled_images_resource_declaration,
                 vulkan_separate_images_resource_declaration,
                 vulkan_separate_samplers_resource_declaration,
                 vulkan_storage_images_resource_declaration,
                 vulkan_acceleration_structures_resource_declaration>;

const auto downcast_vulkan_shader_resource =
    [](const auto& x) -> const vulkan_shader_resource_declaration_base& {
  return x;
};

struct vulkan_uniform_buffer_resource_instance {
  VkDescriptorBufferInfo& GetDescriptorBufferInfo;
};

struct vulkan_storage_buffers_resource_instance {};

struct vulkan_sampled_images_resource_instance {};

struct vulkan_separate_images_resource_instance {};

struct vulkan_separate_samplers_resource_instance {};

struct vulkan_storage_images_resource_instance {};

struct vulkan_acceleration_structures_resource_instance {};

using vulkan_shader_resource_instance = std::variant<
    std::reference_wrapper<vulkan_uniform_buffer_resource_instance>,
    std::reference_wrapper<vulkan_storage_buffers_resource_instance>,
    std::reference_wrapper<vulkan_sampled_images_resource_instance>,
    std::reference_wrapper<vulkan_separate_images_resource_instance>,
    std::reference_wrapper<vulkan_separate_samplers_resource_instance>,
    std::reference_wrapper<vulkan_storage_images_resource_instance>,
    std::reference_wrapper<vulkan_acceleration_structures_resource_instance>>;

using vulkan_descriptor_bindings =
    std::unordered_map<vulkan_descriptor_set_bind_identifier,
                       vulkan_shader_resource_instance>;

struct vulkan_shader_reflection_data {
  std::unordered_map<vulkan_resource_identifier,
                     vulkan_shader_resource_declaration>
      m_shader_resources_declaration;
  std::uint32_t m_descriptor_sets_count;
};

struct vulkan_extension_data {
  std::string m_name;
  bool m_optional{false};
  void* m_feature_struct{nullptr};
  uint32_t m_version{0};
};

}  // namespace wunder
#endif  // WUNDER_VULKAN_TYPES_H
