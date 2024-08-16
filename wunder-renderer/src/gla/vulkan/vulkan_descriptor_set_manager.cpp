#include "gla/vulkan/vulkan_descriptor_set_manager.h"

#include <expected>
#include <variant>

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_pipeline.h"
#include "gla/vulkan/vulkan_shader.h"

namespace {
enum class descriptor_pool_creator_result {
  OK,
  Error = 1000,
  InvalidResource,
  Warning = 100000
};

class descriptor_pool_creator {
 public:
  std::expected<VkWriteDescriptorSet, descriptor_pool_creator_result>
  operator()(const std::monostate&) {}
};
}  // namespace

namespace wunder {

void vulkan_descriptor_set_manager::update_resource(
    const vulkan_resource_identifier& set_identifier,
    vulkan_resource resource) {
  auto& shader_resources_declaration =
      m_shader_reflection_data.m_shader_resources_declaration;
  auto found_resource_met_it =
      shader_resources_declaration.find(set_identifier);
  AssertReturnIf(found_resource_met_it == shader_resources_declaration.end(), );

  const vulkan_shader_resource_declaration& resource_declaration = std::visit(
      downcast_vulkan_shader_resource,
      found_resource_met_it->second);

  auto descriptor_bindings_it =
      m_input_resources.find(resource_declaration.m_set);
  AssertReturnIf(descriptor_bindings_it == m_input_resources.end());
  vulkan_descriptor_bindings& descriptor_bindings =
      descriptor_bindings_it->second;
  //TODO:: validate that the declared and provided types are maching!!!
  descriptor_bindings[resource_declaration.m_binding] = resource;
}

void vulkan_descriptor_set_manager::bake() {
  std::vector<VkWriteDescriptorSet> write_descriptors;
  auto& device = vulkan_layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .get_device();
  VkDevice vulkan_logical_device = device.get_vulkan_logical_device();

  for (auto& [descriptor_set_identifier, descriptor_bindings] :
       m_input_resources) {
    write_descriptors.clear();
    for (auto& [descriptor_set_bind_identifier, vulkan_resource] :
         descriptor_bindings) {
      auto result = std::visit(descriptor_pool_creator(), vulkan_resource);
      AssertContinueIf(result.error() != descriptor_pool_creator_result::OK ||
                       !result.has_value());
      write_descriptors.emplace_back(result.value());
    }

    if (!write_descriptors.empty()) {
      WUNDER_INFO_TAG("Renderer", "Render pass update {} descriptors in set {}",
                      write_descriptors.size(), descriptor_set_identifier);
      vkUpdateDescriptorSets(vulkan_logical_device,
                             (uint32_t)write_descriptors.size(),
                             write_descriptors.data(), 0, nullptr);
    }
  }
}

void vulkan_descriptor_set_manager::bind(
    const vulkan_pipeline& pipeline) const {
  auto& device = vulkan_layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .get_device();

  auto& command_pool = device.get_command_pool();

  vkCmdBindDescriptorSets(command_pool.get_current_graphics_command_buffer(),
                          pipeline.get_bind_point(),
                          pipeline.get_vulkan_pipeline_layout(), 0,
                          static_cast<uint32_t>(m_descriptor_sets.size()),
                          m_descriptor_sets.data(), 0, nullptr);
}

void vulkan_descriptor_set_manager::initialize(
    const vulkan_shader& vulkan_shader) {
  // If valid, we can create descriptor sets

  // Create Descriptor Pool
  VkDescriptorPoolSize poolSizes[] = {
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
      {VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1000}};

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  poolInfo.maxSets =
      10 * 3;  // frames in flight should partially determine this
  poolInfo.poolSizeCount = 10;
  poolInfo.pPoolSizes = poolSizes;

  vulkan_context& context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto* const device = context.get_device().get_vulkan_logical_device();

  VK_CHECK_RESULT(
      vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptor_pool));

  // TODO:: More sophisticated clear, free the resources
  m_descriptor_sets.clear();

  // Obviously descriptors manager must be bound to the shader. However
  // currently there is no way of identifying a resource and search for it by
  // its id
  // TODO:: remove this data copy once the above is not true!!!
  m_shader_reflection_data = vulkan_shader.get_shader_reflection_data();

  // allocate decriptor sets
  for (std::uint32_t i = 0;
       i < m_shader_reflection_data.m_descriptor_sets_count; ++i) {
    auto maybe_dsl = vulkan_shader.get_vulkan_descriptor_set_layout(i);
    AssertContinueUnless(maybe_dsl.has_value());

    VkDescriptorSetAllocateInfo descriptor_set_alloc_info = {};
    descriptor_set_alloc_info.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_alloc_info.pSetLayouts = &maybe_dsl.value();
    descriptor_set_alloc_info.descriptorSetCount = 1;
    descriptor_set_alloc_info.descriptorPool = m_descriptor_pool;

    VkDescriptorSet descriptor_set = nullptr;
    VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &descriptor_set_alloc_info,
                                             &descriptor_set));
    m_descriptor_sets.emplace_back(descriptor_set);
  }
}

}  // namespace wunder