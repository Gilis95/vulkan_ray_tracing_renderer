//
// Created by christian on 8/12/24.
//

#ifndef WUNDER_VULKAN_TYPES_H
#define WUNDER_VULKAN_TYPES_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <variant>

namespace wunder {
using vulkan_resource = std::variant<std::monostate>;
using vulkan_resource_identifier = std::string;

using vulkan_descriptor_set_identifier = std::uint32_t;
using vulkan_descriptor_set_bind_identifier = std::uint32_t;
using vulkan_descriptor_bindings =
    std::unordered_map<vulkan_descriptor_set_bind_identifier, vulkan_resource>;

struct vulkan_shader_resource_declaration {
 public:
  vulkan_descriptor_set_identifier m_set = 0;
  vulkan_descriptor_set_bind_identifier m_binding = 0;
  std::uint32_t Count = 0;
};

// resources.uniform_buffers
// ShaderResource::UniformBuffer uniformBuffer;
struct vulkan_uniform_buffer_resource_declaration
    : public vulkan_shader_resource_declaration {};

// resources.storage_buffers
// ShaderResource::StorageBuffer storageBuffer;
struct vulkan_storage_buffers_resource_declaration
    : public vulkan_shader_resource_declaration {};

// resources.sampled_images
// auto& imageSampler = shaderDescriptorSet.ImageSamplers[binding];
struct vulkan_sampled_images_resource_declaration
    : public vulkan_shader_resource_declaration {};

// resources.separate_images
// shaderDescriptorSet.SeparateTextures[binding];
struct vulkan_separate_images_resource_declaration
    : public vulkan_shader_resource_declaration {};

// resources.separate_samplers
// shaderDescriptorSet.SeparateSamplers[binding];
struct vulkan_separate_samplers_resource_declaration
    : public vulkan_shader_resource_declaration {};

// resources.storage_images
// shaderDescriptorSet.StorageImages[binding]
struct vulkan_storage_images_resource_declaration
    : public vulkan_shader_resource_declaration {};

struct vulkan_acceleration_structures_resource_declaration
    : public vulkan_shader_resource_declaration {};

using vulkan_shader_resource =
    std::variant<vulkan_uniform_buffer_resource_declaration,
                 vulkan_storage_buffers_resource_declaration,
                 vulkan_sampled_images_resource_declaration,
                 vulkan_separate_images_resource_declaration,
                 vulkan_separate_samplers_resource_declaration,
                 vulkan_storage_images_resource_declaration,
                 vulkan_acceleration_structures_resource_declaration>;

const auto downcast_vulkan_shader_resource =
    [](const auto& x) -> const vulkan_shader_resource_declaration& {
  return x;
};

struct vulkan_shader_reflection_data {
  std::unordered_map<vulkan_resource_identifier, vulkan_shader_resource>
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
