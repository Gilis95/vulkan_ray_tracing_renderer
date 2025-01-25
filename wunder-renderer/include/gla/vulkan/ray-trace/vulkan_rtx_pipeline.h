#ifndef WUNDER_VULKAN_RTX_PIPELINE_H
#define WUNDER_VULKAN_RTX_PIPELINE_H

#include "core/non_copyable.h"
#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_base_pipeline.h"

#include <glad/vulkan.h>

namespace wunder::vulkan {
class shader;

class rtx_pipeline : public base_pipeline, public non_copyable{
 public:
  rtx_pipeline();
 public:
  void initialize_pipeline(const vector_map<VkShaderStageFlagBits,
                                        std::vector<unique_ptr<shader>>>&
                           shaders_of_types);

 public:
  [[nodiscard]] const VkRayTracingPipelineCreateInfoKHR&
  get_pipeline_create_info() const {
    return m_pipeline_create_info;
  }

 protected:
  [[nodiscard]] VkPushConstantRange get_push_constant_range() const override;
 private:
  [[nodiscard]] std::vector<VkRayTracingShaderGroupCreateInfoKHR>
  get_shader_group_create_info(
      std::vector<VkPipelineShaderStageCreateInfo>& stages) const;

 private:
  VkRayTracingPipelineCreateInfoKHR m_pipeline_create_info = {
      VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
};
}  // namespace wunder

#endif  // WUNDER_VULKAN_RTX_PIPELINE_H
