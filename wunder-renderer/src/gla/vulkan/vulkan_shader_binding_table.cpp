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

vulkan_shader_binding_table::~vulkan_shader_binding_table() = default;

void vulkan_shader_binding_table::initialize(const vulkan_pipeline& pipeline) {
  initialize_shader_indices(pipeline);
  std::array<std::vector<uint8_t>, 4> shader_stages_handles =
      create_shader_stages_handles(pipeline);
  create_sbt_buffer(shader_stages_handles);
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

std::array<std::vector<uint8_t>, 4>
vulkan_shader_binding_table::create_shader_stages_handles(
    const vulkan_pipeline& pipeline) {  // Fetch all the shader handles used in
                                        // the pipeline, so that they can be

  // Get the total number of groups and handle index position
  const auto& pipeline_create_info = pipeline.get_pipeline_create_info();
  std::uint32_t total_group_count = pipeline_create_info.groupCount;

  auto& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();

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
      m_shader_stage_stride[raygen] * m_shader_handles_indeces[raygen].size());
  shader_stages_handles[miss] = std::vector<uint8_t>(
      m_shader_stage_stride[miss] * m_shader_handles_indeces[miss].size());
  shader_stages_handles[hit] = std::vector<uint8_t>(
      m_shader_stage_stride[hit] * m_shader_handles_indeces[hit].size());
  shader_stages_handles[callable] = std::vector<uint8_t>(
      m_shader_stage_stride[hit] * m_shader_handles_indeces[hit].size());

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
  copy_handles(shader_stages_handles[raygen], m_shader_handles_indeces[raygen],
               m_shader_stage_stride[raygen]);
  copy_handles(shader_stages_handles[miss], m_shader_handles_indeces[miss],
               m_shader_stage_stride[miss]);
  copy_handles(shader_stages_handles[hit], m_shader_handles_indeces[hit],
               m_shader_stage_stride[hit]);
  copy_handles(shader_stages_handles[callable],
               m_shader_handles_indeces[callable],
               m_shader_stage_stride[callable]);
  return shader_stages_handles;
}

void vulkan_shader_binding_table::create_sbt_buffer(
    const std::array<std::vector<uint8_t>, 4>& shader_stages_handles) {
  auto& vulkan_context =
      vulkan_layer_abstraction_factory::instance().get_vulkan_context();
  auto& device = vulkan_context.get_device();
  auto& allocator = vulkan_context.get_resource_allocator();

  for (uint32_t i = 0; i < 4; ++i) {
    auto& shader_stage_handles = shader_stages_handles[i];
    ContinueIf(shader_stage_handles.empty());
    auto & shader_group_buffer = m_shader_group_buffers[i];

    VmaAllocation staging_buffer_allocation;
    VkBuffer staging_buffer;
    unsigned long stage_handles_size =
        shader_stage_handles.size() * sizeof(uint8_t);
    {  // allocate buffer on host side
      VkBufferCreateInfo buffer_create_info{};
      buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      buffer_create_info.size = stage_handles_size;
      buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
      buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      staging_buffer_allocation = allocator.allocate_buffer(
          buffer_create_info, VMA_MEMORY_USAGE_CPU_TO_GPU, staging_buffer);
    }

    {  // Copy data to staging shader_group_buffer
      auto* dest_data =
          allocator.map_memory<uint8_t>(staging_buffer_allocation);
      memcpy(dest_data, shader_stage_handles.data(), stage_handles_size);
      allocator.unmap_memory(staging_buffer_allocation);
    }

    {  // allocate device memory
      VkBufferCreateInfo sbt_buffer_create_info = {};
      sbt_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
      sbt_buffer_create_info.size = stage_handles_size;
      sbt_buffer_create_info.usage =
          VK_BUFFER_USAGE_TRANSFER_DST_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;

      shader_group_buffer.m_allocation = allocator.allocate_buffer(
          sbt_buffer_create_info, VMA_MEMORY_USAGE_GPU_ONLY,
          shader_group_buffer.m_vk_buffer);
    }

    VkCommandBuffer copyCmd =
        device.get_command_pool().get_current_compute_command_buffer();

    VkBufferCopy copyRegion = {};
    copyRegion.size = stage_handles_size;
    vkCmdCopyBuffer(copyCmd, staging_buffer, shader_group_buffer.m_vk_buffer, 1,
                    &copyRegion);

    device.get_command_pool().flush_compute_command_buffer();

    allocator.destroy_buffer(staging_buffer, staging_buffer_allocation);

    set_debug_utils_object_name(device.get_vulkan_logical_device(),
                                std::format("shader_binding_table:{}", i),
                                shader_group_buffer.m_vk_buffer);
  }
}

}  // namespace wunder