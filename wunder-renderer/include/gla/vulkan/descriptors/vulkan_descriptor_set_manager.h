#ifndef WUNDER_VULKAN_DESCRIPTOR_SET_MANAGER_H
#define WUNDER_VULKAN_DESCRIPTOR_SET_MANAGER_H
#include <cstdint>
#include <map>
#include <unordered_map>
#include <variant>
#include <vector>

#include "core/non_copyable.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_shader_types.h"
#include "glad/vulkan.h"

namespace wunder::vulkan {

class shader;
class base_pipeline;

class descriptor_set_manager : public non_copyable {
 public:
  enum build_error_code {
    SUCCESS = 0,
    EMPTY_INPUT = 1,
  };

 public:
  descriptor_set_manager(const vulkan_shader_reflection_data& reflection_data);

 public:
  build_error_code build();

  void bind(const base_pipeline& pipeline) const;
 public:
  void clear_resources();
  void clear_resource(const vulkan_resource_identifier& resource_identifier);
  void add_resource(const vulkan_resource_identifier& resource_identifier,
                    shader_resource::instance::element resource);

  optional_const_ref<shader_resource::declaration::base>
  find_resource_declaration(
      const vulkan_resource_identifier& resource_identifier) const;

  optional_const_ref<vulkan_descriptor_binding> find_resource_binding(
      vulkan_descriptor_set_identifier set,
      vulkan_descriptor_set_bind_identifier bind) const;

 public:
  const std::vector<VkDescriptorSetLayout>& get_descriptor_set_layout() const {
    return m_descriptor_set_layout;
  }

 private:
  std::vector<VkDescriptorSetLayout>& create_descriptor_set_layout();
  void create_descriptors_set();
  void write_descriptors_data();

 private:
  std::map<vulkan_descriptor_set_identifier, vulkan_descriptor_bindings>
      m_input_resources;
  const vulkan_shader_reflection_data& m_shader_reflection_data;
  std::vector<VkDescriptorSet> m_descriptor_sets;

  std::vector<VkDescriptorSetLayout> m_descriptor_set_layout;

  VkDescriptorPool m_descriptor_pool;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_DESCRIPTOR_SET_MANAGER_H
