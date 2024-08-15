//
// Created by christian on 8/12/24.
//
#include "gla/vulkan/vulkan_shader.h"

#include <file_includer.h>
#include <libshaderc_util/file_finder.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <utility>

#include "core/wunder_filesystem.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"

namespace {

inline shaderc_shader_kind vulkan_shader_stage_to_shaderc(
    const VkShaderStageFlagBits stage) {
  switch (stage) {
    case VK_SHADER_STAGE_VERTEX_BIT:
      return shaderc_vertex_shader;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
      return shaderc_fragment_shader;
    case VK_SHADER_STAGE_COMPUTE_BIT:
      return shaderc_compute_shader;
    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
      return shaderc_raygen_shader;
    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
      return shaderc_shader_kind::shaderc_anyhit_shader;
    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
      return shaderc_shader_kind::shaderc_closesthit_shader;
    case VK_SHADER_STAGE_MISS_BIT_KHR:
      return shaderc_shader_kind::shaderc_miss_shader;
    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
      return shaderc_shader_kind::shaderc_intersection_shader;
    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
      return shaderc_shader_kind::shaderc_callable_shader;
  }

  AssertLogIf(true, "Failed to parse shader type");
  return {};
}

inline std::string vulkan_shader_stage_to_string(
    const VkShaderStageFlagBits stage) {
  switch (stage) {
    case VK_SHADER_STAGE_VERTEX_BIT:
      return "VK_SHADER_STAGE_VERTEX_BIT";
    case VK_SHADER_STAGE_FRAGMENT_BIT:
      return "VK_SHADER_STAGE_FRAGMENT_BIT";
    case VK_SHADER_STAGE_COMPUTE_BIT:
      return "VK_SHADER_STAGE_COMPUTE_BIT";
    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
      return "VK_SHADER_STAGE_RAYGEN_BIT_KHR";
    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
      return "VK_SHADER_STAGE_ANY_HIT_BIT_KHR";
    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
      return "VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR";
    case VK_SHADER_STAGE_MISS_BIT_KHR:
      return "VK_SHADER_STAGE_MISS_BIT_KHR";
    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
      return "VK_SHADER_STAGE_INTERSECTION_BIT_KHR";
    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
      return "VK_SHADER_STAGE_CALLABLE_BIT_KHR";
  }

  AssertLogIf(true, "Failed to parse shader type");
  return "";
}
}  // namespace

namespace wunder {
vulkan_shader::vulkan_shader(std::string&& shader_name,
                             VkShaderStageFlagBits vulkan_shader_type)
    : m_shader_name(std::move(shader_name)),
      m_vulkan_shader_type(vulkan_shader_type) {}

std::expected<unique_ptr<vulkan_shader>, shader_compile_result>
vulkan_shader::create(const std::filesystem::path& spirv_path,
                      const VkShaderStageFlagBits stage) {
  auto spirv_real_path = wunder_filesystem::instance().resolve_path(spirv_path);
  AssertReturnUnless(
      std::filesystem::exists(spirv_real_path),
      std::unexpected(shader_compile_result::ShaderFileDoesntExist));

  auto spirv_istream = std::ifstream(spirv_real_path);
  auto shader_name = spirv_real_path.filename().string();
  auto shader_dir = spirv_real_path.parent_path();

  auto expected = compile_shader(spirv_istream, shader_dir, shader_name, stage);
  ReturnIf(!expected.has_value(), std::unexpected(expected.error()));

  auto shader_ptr =
      std::make_unique<vulkan_shader>(std::move(shader_name), stage);
  shader_ptr->initialize(expected.value());
  return shader_ptr;
}

std::expected<std::vector<std::uint32_t>, shader_compile_result>
vulkan_shader::compile_shader(std::ifstream& spirv_istream,
                              const std::filesystem::path& shader_parent_dir,
                              const std::string& shader_name,
                              const VkShaderStageFlagBits stage) {
  static shaderc::Compiler compiler;

  shaderc_util::FileFinder fileFinder;
  fileFinder.search_path().push_back(shader_parent_dir);

  shaderc::CompileOptions compile_options;
  compile_options.SetTargetEnvironment(shaderc_target_env_vulkan,
                                       shaderc_env_version_vulkan_1_3);
  compile_options.SetTargetSpirv(shaderc_spirv_version::shaderc_spirv_version_1_6);
  compile_options.SetWarningsAsErrors();
  compile_options.SetGenerateDebugInfo();
  compile_options.SetIncluder(
      std::make_unique<glslc::FileIncluder>(&fileFinder));

  //  compile_options.SetOptimizationLevel(shaderc_optimization_level_performance);

  std::stringstream shader_string_stream;
  shader_string_stream << spirv_istream.rdbuf();

  const shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
      shader_string_stream.str(), vulkan_shader_stage_to_shaderc(stage),
      shader_name.c_str(), "main", compile_options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
    WUNDER_ERROR_TAG("Renderer", module.GetErrorMessage());
    return std::unexpected(shader_compile_result::CompilationFailed);
  }

  std::vector<uint32_t> binary;
  binary = std::vector<uint32_t>(module.begin(), module.end());
  return binary;
}

void vulkan_shader::initialize(const std::vector<std::uint32_t>& debug_spirv) {
  AssertReturnIf(debug_spirv.empty());
  auto& device = vulkan_layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .get_device();

  VkShaderModuleCreateInfo moduleCreateInfo{};

  moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  moduleCreateInfo.codeSize = debug_spirv.size() * sizeof(uint32_t);
  moduleCreateInfo.pCode = debug_spirv.data();

  VK_CHECK_RESULT(vkCreateShaderModule(device.get_vulkan_logical_device(),
                                       &moduleCreateInfo, NULL,
                                       &m_shader_module));
  set_debug_utils_object_name(
      device.get_vulkan_logical_device(), VK_OBJECT_TYPE_SHADER_MODULE,
      std::format("{}:{}", m_shader_name,
                  vulkan_shader_stage_to_string(m_vulkan_shader_type)),
      m_shader_module);

  initialize_reflection_data(debug_spirv);
}

VkPipelineShaderStageCreateInfo vulkan_shader::get_shader_stage_info() const {
  VkPipelineShaderStageCreateInfo result;
  result.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  result.stage = m_vulkan_shader_type;
  result.module = m_shader_module;
  result.pName = "main";

  return result;
}

void vulkan_shader::initialize_reflection_data(
    const std::vector<std::uint32_t>& debug_spirv) {
  spirv_cross::Compiler compiler(debug_spirv);
  auto spirv_resources = compiler.get_shader_resources();

  auto spirv_resources_to_descriptors_declarations =
      [this,
       &compiler](spirv_cross::SmallVector<spirv_cross::Resource> resources) {
        for (const auto& resource : resources) {
          uint32_t descriptor_set = compiler.get_decoration(
              resource.id, spv::DecorationDescriptorSet);
          uint32_t descriptor_binding =
              compiler.get_decoration(resource.id, spv::DecorationBinding);

          m_reflection_data.m_shader_resources_declaration[resource.name] =
              vulkan_shader_resource_declaration{
                  .m_set = descriptor_set, .m_binding = descriptor_binding};
        }
      };

  spirv_resources_to_descriptors_declarations(spirv_resources.uniform_buffers);
  spirv_resources_to_descriptors_declarations(spirv_resources.storage_buffers);
  spirv_resources_to_descriptors_declarations(spirv_resources.stage_inputs);
  spirv_resources_to_descriptors_declarations(spirv_resources.stage_outputs);
  spirv_resources_to_descriptors_declarations(spirv_resources.subpass_inputs);
  spirv_resources_to_descriptors_declarations(spirv_resources.storage_images);
  spirv_resources_to_descriptors_declarations(spirv_resources.sampled_images);
  spirv_resources_to_descriptors_declarations(spirv_resources.atomic_counters);
  spirv_resources_to_descriptors_declarations(
      spirv_resources.acceleration_structures);
  spirv_resources_to_descriptors_declarations(
      spirv_resources.gl_plain_uniforms);
}
}  // namespace wunder