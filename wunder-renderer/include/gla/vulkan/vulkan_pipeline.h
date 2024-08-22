//
// Created by christian on 8/7/24.
//

#ifndef WUNDER_VULKAN_PIPELINE_H
#define WUNDER_VULKAN_PIPELINE_H

#include <glad/vulkan.h>

#include "core/vector_map.h"
#include "core/wunder_memory.h"

namespace wunder {
class vulkan_shader;

class vulkan_pipeline {
 public:
  void create_pipeline_layout(const vulkan_shader& descriptor_declaring_shader);
  void create_pipeline(const vector_map<VkShaderStageFlagBits,
                                        std::vector<unique_ptr<vulkan_shader>>>&
                           shaders_of_types);

 public:
  [[nodiscard]] VkPipelineLayout get_vulkan_pipeline_layout() const {
    return m_vulkan_pipeline_layout;
  }
  [[nodiscard]] VkPipeline get_vulkan_pipeline() const {
    return m_vulkan_pipeline;
  }

  [[nodiscard]] VkPipelineBindPoint get_bind_point() const {
    return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
  }

  [[nodiscard]] const VkRayTracingPipelineCreateInfoKHR&
  get_pipeline_create_info() const {
    return m_pipeline_create_info;
  }

 private:
  [[nodiscard]] std::vector<VkPipelineShaderStageCreateInfo>
  get_shader_stage_create_info(
      const vector_map<VkShaderStageFlagBits,
                       std::vector<unique_ptr<vulkan_shader>>>&
          shaders_of_types);
  [[nodiscard]] std::vector<VkRayTracingShaderGroupCreateInfoKHR>
  get_shader_group_create_info(
      std::vector<VkPipelineShaderStageCreateInfo>& stages) const;

 private:
  VkPipeline m_vulkan_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_vulkan_pipeline_layout = VK_NULL_HANDLE;
  VkRayTracingPipelineCreateInfoKHR m_pipeline_create_info = {
      VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
};
}  // namespace wunder

#endif  // WUNDER_VULKAN_PIPELINE_H
