#ifndef WUNDER_VULKAN_RTX_PIPELINE_H
#define WUNDER_VULKAN_RTX_PIPELINE_H

#include <glad/vulkan.h>

#include "core/non_copyable.h"
#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/vulkan_base_pipeline.h"

namespace wunder::vulkan {
class shader;

class rtx_pipeline : public base_pipeline, public non_copyable {
 private:
  rtx_pipeline();

 public:
  static std::unique_ptr<rtx_pipeline> create(
      const descriptor_set_manager& descriptor_set_manager,
      const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
          shaders);

 public:
  [[nodiscard]] const VkRayTracingPipelineCreateInfoKHR&
  get_pipeline_create_info() const {
    return m_pipeline_create_info;
  }

 protected:
  [[nodiscard]] VkPushConstantRange get_push_constant_range() const override;

 private:
  [[nodiscard]] void create_shader_group_info();

  void initialize_pipeline(
      const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
          shaders_of_types);

 private:
  VkRayTracingPipelineCreateInfoKHR m_pipeline_create_info = {
      VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};

  std::vector<VkRayTracingShaderGroupCreateInfoKHR> m_shader_stage_groups;
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_RTX_PIPELINE_H
