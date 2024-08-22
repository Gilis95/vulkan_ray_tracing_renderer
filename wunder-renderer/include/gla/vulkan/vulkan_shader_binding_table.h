//
// Created by christian on 8/7/24.
//

#ifndef WUNDER_VULKAN_SHADER_BINDING_TABLE_H
#define WUNDER_VULKAN_SHADER_BINDING_TABLE_H
#include <glad/vulkan.h>

#include <array>
#include <cstdint>
#include <vector>
namespace wunder {
class vulkan_pipeline;

class vulkan_shader_binding_table {
 private:
  enum shader_stage_type { raygen, miss, hit, callable };

 public:
  vulkan_shader_binding_table() noexcept;

 public:
  void initialize(const vulkan_pipeline& pipeline);

 private:
  void initialize_shader_indices(const vulkan_pipeline& pipeline);

 private:
  std::array<std::vector<std::uint32_t>, 4>
      m_shader_handles_indeces;  // Offset index in pipeline. It consists of
                                 // raygen, miss, hit and callable. The amount
                                 // of shaders won't change
  std::array<std::uint32_t, 4>
      m_shader_stage_stride;  // size of single shader stage handle. The only
                              // ones currently available are: raygen, miss, hit
                              // and callable. IMPORTANT: In case we want to
                              // attach specific data to specific stage, this
                              // will be included in stage stride size. However
                              // this is not supported at the moment

  std::array<VkBuffer, 4>
      m_shader_group_buffers;  // buffer resembles handler to shader in group +
                               // data given to sbt

  VkPhysicalDeviceRayTracingPipelinePropertiesKHR m_vulkan_pipeline_properties{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR};
};

}  // namespace wunder
#endif  // WUNDER_VULKAN_SHADER_BINDING_TABLE_H
