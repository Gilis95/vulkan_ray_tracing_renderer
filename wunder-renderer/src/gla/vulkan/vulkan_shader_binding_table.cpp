#include "gla/vulkan/vulkan_shader_binding_table.h"

#include <cstdint>
#include <vector>

#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_device_buffer.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "include/gla/vulkan/ray-trace/vulkan_rtx_pipeline.h"

namespace wunder::vulkan {
namespace {
template <typename integral>
constexpr integral align_up(integral x, size_t a) noexcept {
  return integral((x + (integral(a) - 1)) & ~integral(a - 1));
}

}  // namespace
shader_binding_table::shader_binding_table() noexcept {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.mutable_physical_device();

  VkPhysicalDeviceProperties2 device_properties{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
  device_properties.pNext = &m_vulkan_pipeline_properties;
  vkGetPhysicalDeviceProperties2(physical_device.get_vulkan_physical_device(),
                                 &device_properties);
}

shader_binding_table::~shader_binding_table() = default;

unique_ptr<shader_binding_table> shader_binding_table::create(
    const rtx_pipeline& pipeline) {
  unique_ptr<shader_binding_table> shader_binding_table;
  shader_binding_table.reset(new wunder::vulkan::shader_binding_table());

  shader_binding_table->initialize(pipeline);

  return std::move(shader_binding_table);
}

void shader_binding_table::initialize(const rtx_pipeline& pipeline) {
  initialize_shader_indices(pipeline);
  std::array<std::vector<uint8_t>, 4> shader_stages_handles =
      create_shader_stages_handles(pipeline);
  create_sbt_buffer(shader_stages_handles);
}

VkStridedDeviceAddressRegionKHR shader_binding_table::get_stage_address(
    shader_stage_type type) const {
  auto& shader_group = m_shader_group_buffers[type];
  auto stride = m_shader_stage_stride[type];
  auto indices = m_shader_handles_indices[type];
  return VkStridedDeviceAddressRegionKHR{
      .deviceAddress = shader_group->get_address(),
      .stride = stride,
      .size = indices.size() * stride,};
}

void shader_binding_table::initialize_shader_indices(
    const rtx_pipeline& pipeline) {
  for (auto& i : m_shader_handles_indices) i = {};

  const auto& info = pipeline.get_pipeline_create_info();

  // Finding the handle position of each group, splitting by raygen, miss and
  // hit group
  for (uint32_t i = 0; i < info.groupCount; ++i) {
    if (info.pGroups[i].type == VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR) {
      uint32_t genShader = info.pGroups[i].generalShader;
      AssertContinueUnless(genShader < info.stageCount);

      const VkPipelineShaderStageCreateInfo&
          vk_pipeline_shader_stage_create_info = info.pStages[genShader];

      if (vk_pipeline_shader_stage_create_info.stage &
          VK_SHADER_STAGE_RAYGEN_BIT_KHR) {
        m_shader_handles_indices[shader_stage_type::raygen].push_back(i);
        continue;
      }
      if (vk_pipeline_shader_stage_create_info.stage &
          VK_SHADER_STAGE_MISS_BIT_KHR) {
        m_shader_handles_indices[shader_stage_type::miss].push_back(i);
        continue;
      }
      if (vk_pipeline_shader_stage_create_info.stage &
          VK_SHADER_STAGE_CALLABLE_BIT_KHR) {
        m_shader_handles_indices[shader_stage_type::callable].push_back(i);
        continue;
      }
    } else {
      m_shader_handles_indices[shader_stage_type::hit].push_back(i);
    }
  }
}

std::array<std::vector<uint8_t>, 4>
shader_binding_table::create_shader_stages_handles(
    const rtx_pipeline& pipeline) {  // Fetch all the shader handles used in
                                     // the pipeline, so that they can be

  // Get the total number of groups and handle index position
  const auto& pipeline_create_info = pipeline.get_pipeline_create_info();
  std::uint32_t total_group_count = pipeline_create_info.groupCount;

  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();

  // written in the SBT
  uint32_t sbt_size =
      total_group_count * m_vulkan_pipeline_properties.shaderGroupHandleSize;
  std::vector<uint8_t> shaderHandleStorage(sbt_size);

  VK_CHECK_RESULT(vkGetRayTracingShaderGroupHandlesKHR(
      device.get_vulkan_logical_device(), pipeline.get_vulkan_pipeline(), 0,
      total_group_count, sbt_size, shaderHandleStorage.data()));
  // We have no additional data attached to each shader handle, so
  // we can skip calculation stride of each shader_stages_handles based on
  // attached to it custom data
  uint32_t aligned_handle_size =
      align_up(m_vulkan_pipeline_properties.shaderGroupHandleSize,
               m_vulkan_pipeline_properties.shaderGroupHandleAlignment);
  // Special case, all Raygen must start aligned on GroupBase
  m_shader_stage_stride[raygen] =
      align_up(aligned_handle_size,
               m_vulkan_pipeline_properties.shaderGroupBaseAlignment);

  m_shader_stage_stride[miss] = aligned_handle_size;
  m_shader_stage_stride[hit] = aligned_handle_size;
  m_shader_stage_stride[callable] = aligned_handle_size;

  // Buffer holding the staging information
  std::array<std::vector<uint8_t>, 4> shader_stages_handles;
  shader_stages_handles[raygen] = std::vector<uint8_t>(
      m_shader_stage_stride[raygen] * m_shader_handles_indices[raygen].size());
  shader_stages_handles[miss] = std::vector<uint8_t>(
      m_shader_stage_stride[miss] * m_shader_handles_indices[miss].size());
  shader_stages_handles[hit] = std::vector<uint8_t>(
      m_shader_stage_stride[hit] * m_shader_handles_indices[hit].size());
  shader_stages_handles[callable] = std::vector<uint8_t>(
      m_shader_stage_stride[hit] * m_shader_handles_indices[hit].size());

  // Write the handles in the SBT buffer + data info (if any)
  auto copy_handles = [&](std::vector<uint8_t>& buffer,
                          const std::vector<uint32_t>& shader_indeces,
                          const uint32_t stride) {
    auto* pBuffer = buffer.data();
    for (unsigned int shader_index : shader_indeces) {
      auto* pStart = pBuffer;
      // Copy the handle
      memcpy(pBuffer,
             shaderHandleStorage.data() +
                 (shader_index *
                  m_vulkan_pipeline_properties.shaderGroupHandleSize),
             m_vulkan_pipeline_properties.shaderGroupHandleSize);
      pBuffer = pStart + stride;  // Jumping to next group
    }
  };

  // Copy the handles/data to each staging buffer
  copy_handles(shader_stages_handles[raygen], m_shader_handles_indices[raygen],
               m_shader_stage_stride[raygen]);
  copy_handles(shader_stages_handles[miss], m_shader_handles_indices[miss],
               m_shader_stage_stride[miss]);
  copy_handles(shader_stages_handles[hit], m_shader_handles_indices[hit],
               m_shader_stage_stride[hit]);
  copy_handles(shader_stages_handles[callable],
               m_shader_handles_indices[callable],
               m_shader_stage_stride[callable]);
  return shader_stages_handles;
}

void shader_binding_table::create_sbt_buffer(
    const std::array<std::vector<uint8_t>, 4>& shader_stages_handles) {
  auto& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.mutable_device();

  for (uint32_t i = 0; i < 4; ++i) {
    auto& shader_stage_handles = shader_stages_handles[i];
    AssertContinueIf(shader_stage_handles.empty());
    m_shader_group_buffers[i] = std::make_unique<storage_device_buffer>(
        descriptor_build_data{.m_enabled = false}, shader_stage_handles.data(),
        shader_stage_handles.size() * sizeof(uint8_t),
        VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
            VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR);

    set_debug_utils_object_name(device.get_vulkan_logical_device(),
                                std::format("shader_binding_table:{}", i),
                                m_shader_group_buffers[i]->get_buffer());
  }
}

}  // namespace wunder::vulkan