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

#include "core/vector_map.h"
#include "core/wunder_filesystem.h"
#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_types.h"

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

template <typename T>
concept vulkan_shader_resource_concept =
    std::is_base_of<wunder::vulkan_shader_resource_declaration_base, T>::value;

template <vulkan_shader_resource_concept resource_type>
void spirv_resources_to_descriptors_declarations(
    const spirv_cross::Compiler& compiler,
    const spirv_cross::SmallVector<spirv_cross::Resource>& resources,
    wunder::vulkan_shader_reflection_data& out_reflection_data) {
  for (const auto& resource : resources) {
    uint32_t descriptor_set =
        compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
    uint32_t descriptor_binding =
        compiler.get_decoration(resource.id, spv::DecorationBinding);

    out_reflection_data.m_shader_resources_declaration[resource.name] =
        resource_type{descriptor_set, descriptor_binding};
  }
}

class descriptor_set_layout_creator {
 public:
  VkDescriptorSetLayoutBinding operator()(
      const wunder::vulkan_uniform_buffer_resource_declaration& resource) {
    VkDescriptorSetLayoutBinding layout_binding;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_ALL;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.binding = resource.m_binding;
    return layout_binding;
  }

  VkDescriptorSetLayoutBinding operator()(
      const wunder::vulkan_storage_buffers_resource_declaration& resource) {
    VkDescriptorSetLayoutBinding layout_binding;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_ALL;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.binding = resource.m_binding;
    return layout_binding;
  }

  VkDescriptorSetLayoutBinding operator()(
      const wunder::vulkan_sampled_images_resource_declaration& resource) {
    VkDescriptorSetLayoutBinding layout_binding;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.binding = resource.m_binding;
    return layout_binding;
  }

  VkDescriptorSetLayoutBinding operator()(
      const wunder::vulkan_separate_images_resource_declaration& resource) {
    VkDescriptorSetLayoutBinding layout_binding;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.binding = resource.m_binding;
    return layout_binding;
  }

  VkDescriptorSetLayoutBinding operator()(
      const wunder::vulkan_separate_samplers_resource_declaration& resource) {
    VkDescriptorSetLayoutBinding layout_binding;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.binding = resource.m_binding;
    return layout_binding;
  }

  VkDescriptorSetLayoutBinding operator()(
      const wunder::vulkan_storage_images_resource_declaration& resource) {
    VkDescriptorSetLayoutBinding layout_binding;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.binding = resource.m_binding;
    return layout_binding;
  }

  VkDescriptorSetLayoutBinding operator()(
      const wunder::vulkan_acceleration_structures_resource_declaration&
          resource) {
    VkDescriptorSetLayoutBinding layout_binding;
    layout_binding.descriptorType =
        VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    layout_binding.descriptorCount = 1;
    layout_binding.stageFlags = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
    layout_binding.pImmutableSamplers = nullptr;
    layout_binding.binding = resource.m_binding;
    return layout_binding;
  }
};
}  // namespace

namespace wunder {
vulkan_shader::vulkan_shader(std::string&& shader_name,
                             VkShaderStageFlagBits vulkan_shader_type)
    : m_shader_name(std::move(shader_name)),
      m_vulkan_shader_type(vulkan_shader_type) {}

std::expected<unique_ptr<vulkan_shader>, shader_operation_output_code>
vulkan_shader::create(const std::filesystem::path& spirv_path,
                      const VkShaderStageFlagBits stage) {
  auto spirv_real_path = wunder_filesystem::instance().resolve_path(spirv_path);
  AssertReturnUnless(
      std::filesystem::exists(spirv_real_path),
      std::unexpected(shader_operation_output_code::ShaderFileDoesntExist));

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

std::expected<std::vector<std::uint32_t>, shader_operation_output_code>
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
  compile_options.SetTargetSpirv(
      shaderc_spirv_version::shaderc_spirv_version_1_6);
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
    return std::unexpected(shader_operation_output_code::CompilationFailed);
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
  initialize_descriptor_set_layout();
}

VkPipelineShaderStageCreateInfo vulkan_shader::get_shader_stage_info() const {
  VkPipelineShaderStageCreateInfo result{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
  result.stage = m_vulkan_shader_type;
  result.flags = 0;
  result.module = m_shader_module;
  result.pName = "main";

  return result;
}

void vulkan_shader::initialize_reflection_data(
    const std::vector<std::uint32_t>& debug_spirv) {
  spirv_cross::Compiler compiler(debug_spirv);
  auto resources = compiler.get_shader_resources();

  spirv_resources_to_descriptors_declarations<
      vulkan_uniform_buffer_resource_declaration>(
      compiler, resources.uniform_buffers, m_reflection_data);
  spirv_resources_to_descriptors_declarations<
      vulkan_storage_buffers_resource_declaration>(
      compiler, resources.storage_buffers, m_reflection_data);
  spirv_resources_to_descriptors_declarations<
      vulkan_sampled_images_resource_declaration>(
      compiler, resources.sampled_images, m_reflection_data);
  spirv_resources_to_descriptors_declarations<
      vulkan_separate_images_resource_declaration>(
      compiler, resources.separate_images, m_reflection_data);
  spirv_resources_to_descriptors_declarations<
      vulkan_separate_samplers_resource_declaration>(
      compiler, resources.separate_samplers, m_reflection_data);
  spirv_resources_to_descriptors_declarations<
      vulkan_storage_images_resource_declaration>(
      compiler, resources.storage_images, m_reflection_data);
  spirv_resources_to_descriptors_declarations<
      vulkan_acceleration_structures_resource_declaration>(
      compiler, resources.acceleration_structures, m_reflection_data);

  ReturnIf(m_reflection_data.m_shader_resources_declaration.empty());

  auto resource_with_max_set = std::max_element(
      m_reflection_data.m_shader_resources_declaration.begin(),
      m_reflection_data.m_shader_resources_declaration.end(),
      [](const std::pair<vulkan_resource_identifier,
                         vulkan_shader_resource_declaration>&
             left_element,
         const std::pair<vulkan_resource_identifier,
                         vulkan_shader_resource_declaration>&
             right_element) {
        const vulkan_shader_resource_declaration_base& left_resource_declaration = std::visit(
            downcast_vulkan_shader_resource, left_element.second);
        const vulkan_shader_resource_declaration_base& right_resource_declaration = std::visit(
            downcast_vulkan_shader_resource, right_element.second);

        return left_resource_declaration.m_set < right_resource_declaration.m_set;
      });

  m_reflection_data.m_descriptor_sets_count = std::visit(
      downcast_vulkan_shader_resource, resource_with_max_set->second).m_set + 1;
}

void vulkan_shader::initialize_descriptor_set_layout() {
  vector_map<vulkan_descriptor_set_identifier,
             std::vector<VkDescriptorSetLayoutBinding>>
      per_set_layout_bindings;

  auto layout_creator = descriptor_set_layout_creator();
  for (auto& [_, resource_declaration_variant] :
       m_reflection_data.m_shader_resources_declaration) {
    const vulkan_shader_resource_declaration_base& resource_declaration = std::visit(
        downcast_vulkan_shader_resource, resource_declaration_variant);

    auto& layout_bindings = per_set_layout_bindings[resource_declaration.m_set];
    layout_bindings.emplace_back(std::visit(layout_creator,
                                            resource_declaration_variant));
  }

  auto& device = vulkan_layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .get_device();
  VkDevice vulkan_logical_device = device.get_vulkan_logical_device();

  for (auto& [set_identifier, layout_bindings] : per_set_layout_bindings) {
    VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
    descriptorLayout.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayout.pNext = nullptr;
    descriptorLayout.bindingCount = (uint32_t)(layout_bindings.size());
    descriptorLayout.pBindings = layout_bindings.data();

    if (set_identifier >= m_descriptor_set_layout.size())
      m_descriptor_set_layout.resize(set_identifier + 1);
    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(
        vulkan_logical_device, &descriptorLayout, nullptr,
        &m_descriptor_set_layout[set_identifier]));
  }
}

std::expected<VkDescriptorSetLayout, shader_operation_output_code>
vulkan_shader::get_vulkan_descriptor_set_layout(
    vulkan_descriptor_set_identifier set) const {
  AssertReturnIf(m_descriptor_set_layout.size() <= set,
                 std::unexpected(shader_operation_output_code::NoSetAllocated));

  return m_descriptor_set_layout[set];
}
}  // namespace wunder