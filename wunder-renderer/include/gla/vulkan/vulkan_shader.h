#ifndef WUNDER_VULKAN_SHADER_H
#define WUNDER_VULKAN_SHADER_H

#include <glad/vulkan.h>

#include <expected>
#include <filesystem>
#include <vector>

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_types.h"

namespace wunder {
enum class shader_compile_result {
  OK,
  Warning = 1000,
  Error = 100000,
  ShaderFileDoesntExist,
  CompilationFailed
};

class vulkan_shader {
 public:
  explicit vulkan_shader(std::string&& shader_name,
                         VkShaderStageFlagBits vulkan_shader_type);

 public:
  static std::expected<unique_ptr<vulkan_shader>, shader_compile_result> create(
      const std::filesystem::path& spirv, const VkShaderStageFlagBits stage);

  static std::expected<std::vector<std::uint32_t>, shader_compile_result>
  compile_shader(std::ifstream& spirv_istream,const std::filesystem::path& shader_parent_dir, const std::string& shader_name,
                 const VkShaderStageFlagBits stage);

 public:
  void initialize(const std::vector<std::uint32_t>& debug_spirv);

 public:
  const vulkan_shader_reflection_data& get_shader_reflection_data() const {
    return m_reflection_data;
  }
  VkDescriptorSetLayout get_vulkan_descriptor_set_layout() const {
    return m_descriptor_set_layout;
  }
  VkShaderModule get_vulkan_shader_module() const { return m_shader_module; }

 private:
  void initialize_reflection_data(
      const std::vector<std::uint32_t>& debug_spirv);

 private:
  std::string m_shader_name;
  vulkan_shader_reflection_data m_reflection_data;

  const VkShaderStageFlagBits m_vulkan_shader_type;
  VkDescriptorSetLayout m_descriptor_set_layout = VK_NULL_HANDLE;
  VkShaderModule m_shader_module = VK_NULL_HANDLE;
  VkPipelineShaderStageCreateInfo get_shader_stage_info() const;
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_SHADER_H