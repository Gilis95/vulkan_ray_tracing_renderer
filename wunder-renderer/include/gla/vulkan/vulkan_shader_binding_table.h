//
// Created by christian on 8/7/24.
//

#ifndef WUNDER_VULKAN_SHADER_BINDING_TABLE_H
#define WUNDER_VULKAN_SHADER_BINDING_TABLE_H
#include <glad/vulkan.h>

#include <array>
#include <cstdint>
#include <vector>

#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_buffer_fwd.h"

namespace wunder::vulkan {
class rtx_pipeline;

class shader_binding_table {
 public:
  enum shader_stage_type { raygen, miss, hit, callable };

 private:
  shader_binding_table() noexcept;

 public:
  ~shader_binding_table();

  static unique_ptr<shader_binding_table> create(const rtx_pipeline& pipeline);

 public:
  VkStridedDeviceAddressRegionKHR get_stage_address(
      shader_stage_type type) const;

 private:
  void initialize(const rtx_pipeline& pipeline);

  void initialize_shader_indices(const rtx_pipeline& pipeline);

  std::array<std::vector<uint8_t>, 4> create_shader_stages_handles(
      const rtx_pipeline& pipeline);
  void create_sbt_buffer(
      const std::array<std::vector<uint8_t>, 4>& shader_stages_handles);

 private:
  std::array<std::vector<std::uint32_t>, 4>
      m_shader_handles_indices{};  // Offset index in pipeline. It consists of
                                   // raygen, miss, hit and callable. The amount
                                   // of shaders won't change
  std::array<std::uint32_t, 4>
      m_shader_stage_stride{};  // size of single shader stage handle. The only
                                // ones currently available are: raygen, miss,
                                // hit and callable. IMPORTANT: In case we want
                                // to attach specific data to specific stage,
                                // this will be included in stage stride size.
                                // However this is not supported at the moment

  std::array<unique_ptr<storage_buffer>, 4>
      m_shader_group_buffers;  // buffer resembles handler to shader in group
                               // + data given to sbt

  VkPhysicalDeviceRayTracingPipelinePropertiesKHR
      m_vulkan_pipeline_properties{};
};

}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_SHADER_BINDING_TABLE_H
