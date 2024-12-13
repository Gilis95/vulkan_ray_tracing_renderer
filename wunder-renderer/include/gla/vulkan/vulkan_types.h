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

namespace wunder::vulkan {
using vulkan_resource_identifier = std::string;

using vulkan_descriptor_set_identifier = std::uint32_t;
using vulkan_descriptor_set_bind_identifier = std::uint32_t;

class renderer;

namespace shader_resource {

namespace declaration {

struct base {
 public:
  vulkan_descriptor_set_identifier m_set = 0;
  vulkan_descriptor_set_bind_identifier m_binding = 0;
  std::uint32_t Count = 0;
};

// resources.uniform_buffers
// ShaderResource::UniformBuffer uniformBuffer;
struct uniform_buffer : public base {};

// resources.storage_buffers
// ShaderResource::StorageBuffer storageBuffer;
struct storage_buffers : public base {};

// resources.sampled_images
// auto& imageSampler = shaderDescriptorSet.ImageSamplers[binding];
struct sampled_images : public base {};

// resources.separate_images
// shaderDescriptorSet.SeparateTextures[binding];
struct separate_images : public base {};

// resources.separate_samplers
// shaderDescriptorSet.SeparateSamplers[binding];
struct separate_samplers : public base {};

// resources.storage_images
// shaderDescriptorSet.StorageImages[binding]
struct storage_images : public base {};

struct acceleration_structures : public base {};

using element = std::variant<uniform_buffer, storage_buffers, sampled_images,
                             separate_images, separate_samplers, storage_images,
                             acceleration_structures>;

const auto downcast = [](const auto& x) -> const base& { return x; };

}  // namespace declaration

namespace instance {
struct base {
  virtual void bind(renderer& renderer) = 0;
};

struct uniform_buffer : public base {
  VkDescriptorBufferInfo& GetDescriptorBufferInfo;
};

struct storage_buffers : public base {};

struct sampled_images : public base {};

struct separate_images : public base {};

struct separate_samplers : public base {};

struct storage_images : public base {};

class acceleration_structures : public base {
 public:
  void assign_acceleration_structure(VkAccelerationStructureKHR structure);

 public:
  VkWriteDescriptorSetAccelerationStructureKHR
      m_descriptor_set_acceleration_structure_khr;
};

using element = std::variant<std::reference_wrapper<uniform_buffer>,
                             std::reference_wrapper<storage_buffers>,
                             std::reference_wrapper<sampled_images>,
                             std::reference_wrapper<separate_images>,
                             std::reference_wrapper<separate_samplers>,
                             std::reference_wrapper<storage_images>,
                             std::reference_wrapper<acceleration_structures>>;

}  // namespace instance

}  // namespace shader::resource

using vulkan_descriptor_bindings =
    std::unordered_map<vulkan_descriptor_set_bind_identifier,
                       shader_resource::instance::element>;

struct vulkan_shader_reflection_data {
  std::unordered_map<vulkan_resource_identifier,
                     shader_resource::declaration::element>
      m_shader_resources_declaration;
  std::uint32_t m_descriptor_sets_count;
};

struct vulkan_extension_data {
  std::string m_name;
  bool m_optional{false};
  void* m_feature_struct{nullptr};
  uint32_t m_version{0};
};

}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_TYPES_H
