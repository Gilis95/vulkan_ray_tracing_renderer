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
  vulkan_descriptor_set_identifier m_set = 0;
  vulkan_descriptor_set_bind_identifier m_binding = 0;
  std::uint32_t Count = 0;
};

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
