#ifndef WUNDER_VULKAN_SHADER_H
#define WUNDER_VULKAN_SHADER_H

#include <glad/vulkan.h>

#include <expected>
#include <filesystem>
#include <vector>

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_types.h"

namespace wunder {
enum class shader_operation_output_code {
  OK,
  Warning = 1000,
  Error = 100000,
  ShaderFileDoesntExist,
  CompilationFailed,
  NoSetAllocated
};

class vulkan_shader {
 public:
  explicit vulkan_shader(std::string&& shader_name,
                         VkShaderStageFlagBits vulkan_shader_type);

 public:
  static std::expected<unique_ptr<vulkan_shader>, shader_operation_output_code>
  create(const std::filesystem::path& spirv, const VkShaderStageFlagBits stage);

  static std::expected<std::vector<std::uint32_t>, shader_operation_output_code>
  compile_shader(std::ifstream& spirv_istream,
                 const std::filesystem::path& shader_parent_dir,
                 const std::string& shader_name,
                 const VkShaderStageFlagBits stage);

 public:
  void initialize(const std::vector<std::uint32_t>& debug_spirv);

 public:
  const vulkan_shader_reflection_data& get_shader_reflection_data() const {
    return m_reflection_data;
  }
  std::expected<VkDescriptorSetLayout, shader_operation_output_code>
  get_vulkan_descriptor_set_layout(vulkan_descriptor_set_identifier set) const;

  VkShaderModule get_vulkan_shader_module() const { return m_shader_module; }

  VkPipelineShaderStageCreateInfo get_shader_stage_info() const;

  const std::vector<VkDescriptorSetLayout>& get_descriptor_set_layout() const {
    return m_descriptor_set_layout;
  }

 private:
  void initialize_reflection_data(
      const std::vector<std::uint32_t>& debug_spirv);
  void initialize_descriptor_set_layout();

 private:
  std::string m_shader_name;
  vulkan_shader_reflection_data m_reflection_data;

  const VkShaderStageFlagBits m_vulkan_shader_type;
  std::vector<VkDescriptorSetLayout> m_descriptor_set_layout;
  VkShaderModule m_shader_module = VK_NULL_HANDLE;
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_SHADER_H