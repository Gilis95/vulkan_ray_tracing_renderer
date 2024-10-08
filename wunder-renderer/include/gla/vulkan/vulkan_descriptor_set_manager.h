#ifndef WUNDER_VULKAN_DESCRIPTOR_SET_MANAGER_H
#define WUNDER_VULKAN_DESCRIPTOR_SET_MANAGER_H
#include <cstdint>
#include <map>
#include <unordered_map>
#include <variant>
#include <vector>

#include "core/non_copyable.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_types.h"
#include "glad/vulkan.h"

namespace wunder {

class vulkan_shader;
class vulkan_pipeline;

class vulkan_descriptor_set_manager : public non_copyable{
 public:
  void initialize(const vulkan_shader& vulkan_shader);

 public:

  void update_resource(const vulkan_resource_identifier& resource_identifier,
                       vulkan_shader_resource_instance);

  void bake();
  void bind(const vulkan_pipeline& pipeline) const;
 private:
  std::multimap<vulkan_descriptor_set_identifier, vulkan_descriptor_bindings>
      m_input_resources;
  vulkan_shader_reflection_data m_shader_reflection_data;
  std::vector<VkDescriptorSet> m_descriptor_sets;

  VkDescriptorPool m_descriptor_pool;
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_DESCRIPTOR_SET_MANAGER_H
