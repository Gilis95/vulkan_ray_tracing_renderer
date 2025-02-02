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
  void initialize(const shader& vulkan_shader);

 public:
  void clear_resources();
  void clear_resource(const vulkan_resource_identifier& resource_identifier);
  void add_resource(const vulkan_resource_identifier& resource_identifier,
                    shader_resource::instance::element resource);

  void bake();
  void bind(const base_pipeline& pipeline) const;

  optional_const_ref<shader_resource::declaration::base>
  find_resource_declaration(
      const vulkan_resource_identifier& resource_identifier) const;
 private:
  std::multimap<vulkan_descriptor_set_identifier, vulkan_descriptor_bindings>
      m_input_resources;
  vulkan_shader_reflection_data m_shader_reflection_data;
  std::vector<VkDescriptorSet> m_descriptor_sets;

  VkDescriptorPool m_descriptor_pool;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_DESCRIPTOR_SET_MANAGER_H
