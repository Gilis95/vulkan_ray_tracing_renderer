#include "gla/vulkan/vulkan_shader_binding_table.h"

#include <cstdint>
#include <vector>

#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "gla/vulkan/vulkan_pipeline.h"

namespace wunder {
namespace {
template <typename integral>
constexpr integral align_up(integral x, size_t a) noexcept {
  return integral((x + (integral(a) - 1)) & ~integral(a - 1));
}

}  // namespace
vulkan_shader_binding_table::vulkan_shader_binding_table() noexcept {
  auto& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.get_physical_device();

  VkPhysicalDeviceProperties2 device_properties{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
  device_properties.pNext = &m_vulkan_pipeline_properties;
  vkGetPhysicalDeviceProperties2(physical_device.get_vulkan_physical_device(),
                                 &device_properties);
}

void vulkan_shader_binding_table::initialize(const vulkan_pipeline& pipeline) {
  auto& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  auto& allocator = vulkan_context.get_resource_allocator();

  // Get the total number of groups and handle index position
  const auto& pipeline_create_info = pipeline.get_pipeline_create_info();
  std::uint32_t total_group_count = pipeline_create_info.groupCount;

  initialize_shader_indices(pipeline);

  // Fetch all the shader handles used in the pipeline, so that they can be
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
  m_shader_stage_stride[shader_stage_type::raygen] =
      align_up(aligned_handle_size,
               m_vulkan_pipeline_properties.shaderGroupBaseAlignment);

  m_shader_stage_stride[shader_stage_type::miss] = aligned_handle_size;
  m_shader_stage_stride[shader_stage_type::hit] = aligned_handle_size;
  m_shader_stage_stride[shader_stage_type::callable] = aligned_handle_size;

  // Buffer holding the staging information
  std::array<std::vector<uint8_t>, 4> shader_stages_handles;
  shader_stages_handles[shader_stage_type::raygen] = std::vector<uint8_t>(
      m_shader_stage_stride[shader_stage_type::raygen] *
      m_shader_handles_indeces[shader_stage_type::raygen].size());
  shader_stages_handles[shader_stage_type::miss] = std::vector<uint8_t>(
      m_shader_stage_stride[shader_stage_type::miss] *
      m_shader_handles_indeces[shader_stage_type::miss].size());
  shader_stages_handles[shader_stage_type::hit] = std::vector<uint8_t>(
      m_shader_stage_stride[shader_stage_type::hit] *
      m_shader_handles_indeces[shader_stage_type::hit].size());
  shader_stages_handles[shader_stage_type::callable] = std::vector<uint8_t>(
      m_shader_stage_stride[shader_stage_type::hit] *
      m_shader_handles_indeces[shader_stage_type::hit].size());

  // Write the handles in the SBT buffer + data info (if any)
  auto copyHandles = [&](std::vector<uint8_t>& buffer,
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
  copyHandles(shader_stages_handles[raygen], m_shader_handles_indeces[raygen],
              m_shader_stage_stride[raygen]);
  copyHandles(shader_stages_handles[miss], m_shader_handles_indeces[miss],
              m_shader_stage_stride[miss]);
  copyHandles(shader_stages_handles[hit], m_shader_handles_indeces[hit],
              m_shader_stage_stride[hit]);
  copyHandles(shader_stages_handles[callable],
              m_shader_handles_indeces[callable],
              m_shader_stage_stride[callable]);

  for (uint32_t i = 0; i < 4; ++i) {
    ContinueIf(shader_stages_handles[i].empty());
    VkBufferCreateInfo bufferInfo = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bufferInfo.size = 65536;
    bufferInfo.usage = VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
                       VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

    VkBuffer& buffer = m_shader_group_buffers[i];
    VmaAllocation allocation;
    vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation,
                    nullptr);

    set_debug_utils_object_name(device.get_vulkan_logical_device(),
                                std::format("shader_binding_table:{}", i),
                                buffer);
  }
}

void vulkan_shader_binding_table::initialize_shader_indices(
    const vulkan_pipeline& pipeline) {
  for (auto& i : m_shader_handles_indeces) i = {};

  const auto& info = pipeline.get_pipeline_create_info();

  // Finding the handle position of each group, splitting by raygen, miss and
  // hit group
  for (uint32_t i = 0; i < info.groupCount; ++i) {
    if (info.pGroups[i].type == VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR) {
      uint32_t genShader = info.pGroups[i].generalShader;
      AssertContinueUnless(genShader < info.stageCount);

      if (info.pStages[genShader].stage & VK_SHADER_STAGE_RAYGEN_BIT_KHR) {
        m_shader_handles_indeces[shader_stage_type::raygen].push_back(i);
        continue;
      }
      if (info.pStages[genShader].stage & VK_SHADER_STAGE_MISS_BIT_KHR) {
        m_shader_handles_indeces[shader_stage_type::miss].push_back(i);
        continue;
      }
      if (info.pStages[genShader].stage & VK_SHADER_STAGE_CALLABLE_BIT_KHR) {
        m_shader_handles_indeces[shader_stage_type::callable].push_back(i);
        continue;
      }
    } else {
      m_shader_handles_indeces[shader_stage_type::hit].push_back(i);
    }
  }
}

}  // namespace wunder