#ifndef WUNDER_VULKAN_BASE_PIPELINE_H
#define WUNDER_VULKAN_BASE_PIPELINE_H

#include <glad/vulkan.h>

#include <vector>

#include "core/vector_map.h"
#include "core/wunder_memory.h"
namespace wunder::vulkan {
class shader;

class base_pipeline {
 public:
  base_pipeline(VkPipelineBindPoint bind_point);

 public:
  [[nodiscard]] VkPipelineLayout get_vulkan_pipeline_layout() const {
    return m_vulkan_pipeline_layout;
  }
  [[nodiscard]] VkPipeline get_vulkan_pipeline() const {
    return m_vulkan_pipeline;
  }

  [[nodiscard]] VkPipelineBindPoint get_bind_point() const {
    return m_bind_point;
  }

 public:
  void bind();

 public:
  void initialize_pipeline_layout(const shader& descriptor_declaring_shader);

 protected:
  [[nodiscard]] virtual VkPushConstantRange get_push_constant_range() const = 0;
  std::vector<VkPipelineShaderStageCreateInfo> get_shader_stage_create_info(
      const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
          shaders_of_types);

 protected:
  VkPipeline m_vulkan_pipeline = VK_NULL_HANDLE;
  VkPipelineLayout m_vulkan_pipeline_layout = VK_NULL_HANDLE;
  VkPipelineBindPoint m_bind_point;
};

}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_BASE_PIPELINE_H
