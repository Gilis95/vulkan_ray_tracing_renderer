#include "gla/vulkan/vulkan_descriptor_set_manager.h"

#include <expected>
#include <variant>

#include "core/wunder_macros.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_shader.h"
#include "include/gla/vulkan/vulkan_base_pipeline.h"

namespace {
enum class descriptor_pool_creator_result {
  OK,
  Error = 1000,
  InvalidResource,
  Warning = 100000
};

class write_descriptor_creator {
 public:
  VkWriteDescriptorSet operator()(
      const std::vector<VkDescriptorImageInfo>& resource) {
    VkWriteDescriptorSet result;
    std::memset(&result, 0, sizeof(VkWriteDescriptorSet));

    result.descriptorCount = resource.size();
    result.pImageInfo = resource.data();

    return result;
  }

  VkWriteDescriptorSet operator()(
      const std::vector<VkDescriptorBufferInfo>& resource) {
    VkWriteDescriptorSet result;
    std::memset(&result, 0, sizeof(VkWriteDescriptorSet));

    result.descriptorCount = resource.size();
    result.pBufferInfo = resource.data();

    return result;
  }

  VkWriteDescriptorSet operator()(std::vector<VkBufferView>& resource) {
    VkWriteDescriptorSet result;
    std::memset(&result, 0, sizeof(VkWriteDescriptorSet));

    result.descriptorCount = resource.size();
    result.pTexelBufferView = resource.data();

    return result;
  }

  VkWriteDescriptorSet operator()(
      std::vector<VkAccelerationStructureKHR>& resources) {
    VkWriteDescriptorSet result;
    std::memset(&result, 0, sizeof(VkWriteDescriptorSet));
    ReturnIf(resources.empty(), result);

    // TODO:: this is a workaround. However currently it has no impacts
    //  problematic sections are multi-thread access and having multiple
    //  descriptors of this type, placed in separate bindings
    static VkWriteDescriptorSetAccelerationStructureKHR
        descriptor_set_acceleration_structure;
    std::memset(&descriptor_set_acceleration_structure, 0,
                sizeof(VkWriteDescriptorSetAccelerationStructureKHR));

    descriptor_set_acceleration_structure.sType =
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    descriptor_set_acceleration_structure.accelerationStructureCount =
        resources.size();
    descriptor_set_acceleration_structure.pAccelerationStructures =
        resources.data();

    result.descriptorCount =
        descriptor_set_acceleration_structure.accelerationStructureCount;
    result.pNext = &descriptor_set_acceleration_structure;
    return result;
  }
};
}  // namespace

namespace wunder::vulkan {

void descriptor_set_manager::clear_resources() {
  for (auto& [_, input_resource] : m_input_resources) {
    for (auto& [_, binding] : input_resource.m_bindings) {
      binding.clear_resources();
    }
  }
}

void descriptor_set_manager::clear_resource(
    const vulkan_resource_identifier& resource_identifier) {
  optional_const_ref<shader_resource::declaration::base>
      maybe_resource_declaration =
          find_resource_declaration(resource_identifier);
  AssertReturnUnless(maybe_resource_declaration.has_value());

  const auto& resource_declaration = maybe_resource_declaration->get();

  auto descriptor_bindings_it =
      m_input_resources.find(resource_declaration.m_set);
  ReturnIf(descriptor_bindings_it == m_input_resources.end());

  vulkan_descriptor_bindings& descriptor_bindings =
      descriptor_bindings_it->second;

  descriptor_bindings.clear_resource(resource_declaration.m_binding);
}

// TODO:: This mechanism doesn't provide a way of updating descriptors, other
// than updating everything in the binding
void descriptor_set_manager::add_resource(
    const vulkan_resource_identifier& resource_identifier,
    shader_resource::instance::element resource) {
  optional_const_ref<shader_resource::declaration::base> resource_declaration =
      find_resource_declaration(resource_identifier);
  AssertReturnUnless(resource_declaration.has_value());

  auto descriptor_bindings_it =
      m_input_resources.find(resource_declaration->get().m_set);
  AssertReturnIf(descriptor_bindings_it == m_input_resources.end());
  vulkan_descriptor_bindings& descriptor_bindings =
      descriptor_bindings_it->second;

  // TODO:: validate that the declared and provided types are maching!!!
  descriptor_bindings.emplace_resource(resource_declaration->get().m_binding,
                                       resource);
}

void descriptor_set_manager::bake() {
  std::vector<VkWriteDescriptorSet> write_descriptors;
  auto& device = layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .mutable_device();
  VkDevice vulkan_logical_device = device.get_vulkan_logical_device();
  write_descriptor_creator descriptor_creator;

  for (auto& [descriptor_set_identifier, descriptor_bindings] :
       m_input_resources) {
    AssertContinueUnless(descriptor_set_identifier < m_descriptor_sets.size());
    write_descriptors.clear();

    for (auto& [descriptor_set_bind_identifier, vulkan_resource] :
         descriptor_bindings.m_bindings) {
      VkWriteDescriptorSet result =
          std::visit(descriptor_creator, vulkan_resource.m_resources);
      result.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      result.dstSet = m_descriptor_sets[descriptor_set_identifier];
      result.dstBinding = descriptor_set_bind_identifier;
      result.descriptorType = vulkan_resource.m_descriptor_type;
      // TODO:: No support for descriptors append at the moment
      result.dstArrayElement = 0;

      write_descriptors.emplace_back(std::move(result));
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

void descriptor_set_manager::bind(const base_pipeline& pipeline) const {
  auto command_buffer = layer_abstraction_factory::instance()
                            .get_vulkan_context()
                            .mutable_swap_chain()
                            .get_current_command_buffer();

  vkCmdBindDescriptorSets(command_buffer, pipeline.get_bind_point(),
                          pipeline.get_vulkan_pipeline_layout(), 0,
                          static_cast<uint32_t>(m_descriptor_sets.size()),
                          m_descriptor_sets.data(), 0, nullptr);
}

void descriptor_set_manager::initialize(const shader& vulkan_shader) {
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

  context& context = layer_abstraction_factory::instance().get_vulkan_context();
  auto* const device = context.mutable_device().get_vulkan_logical_device();

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
    m_input_resources.emplace(i, vulkan_descriptor_bindings{});
  }
}
optional_const_ref<shader_resource::declaration::base>
descriptor_set_manager::find_resource_declaration(
    const vulkan_resource_identifier& resource_identifier) const {
  auto& shader_resources_declaration =
      m_shader_reflection_data.m_shader_resources_declaration;

  auto found_resource_declaration_it =
      shader_resources_declaration.find(resource_identifier);
  ReturnIf(found_resource_declaration_it == shader_resources_declaration.end(),
           std::nullopt);

  const shader_resource::declaration::base& resource_declaration =
      std::visit(shader_resource::declaration::downcast,
                 found_resource_declaration_it->second);

  return resource_declaration;
}
}  // namespace wunder::vulkan