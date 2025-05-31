#ifndef WUNDER_VULKAN_SHADER_H
#define WUNDER_VULKAN_SHADER_H

#include <glad/vulkan.h>

#include <expected>
#include <filesystem>
#include <vector>

#include "core/non_copyable.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_shader_types.h"

namespace wunder::vulkan {
enum class shader_operation_output_code {
  OK,
  Warning = 1000,
  Error = 100000,
  ShaderFileDoesntExist,
  CompilationFailed,
  NoSetAllocated
};

class shader : public non_copyable {
 public:
  explicit shader(std::string&& shader_name,
                         VkShaderStageFlagBits vulkan_shader_type);

  ~shader() override;

 public:
  static std::expected<unique_ptr<shader>, shader_operation_output_code>
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

  VkShaderModule get_vulkan_shader_module() const { return m_shader_module; }

  VkPipelineShaderStageCreateInfo get_shader_stage_info() const;

 private:
  void initialize_reflection_data(
      const std::vector<std::uint32_t>& debug_spirv);

 private:
  std::string m_shader_name;
  vulkan_shader_reflection_data m_reflection_data;

  const VkShaderStageFlagBits m_vulkan_shader_type;
  VkShaderModule m_shader_module = VK_NULL_HANDLE;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_SHADER_H