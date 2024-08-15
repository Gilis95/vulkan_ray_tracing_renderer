//
// Created by christian on 8/7/24.
//

#ifndef WUNDER_VULKAN_PIPELINE_H
#define WUNDER_VULKAN_PIPELINE_H

#include <vector>

#include "core/wunder_memory.h"
namespace wunder {
class vulkan_shader;

class vulkan_pipeline {
 public:
  static unique_ptr<vulkan_pipeline> create_ray_tracing_pipeline(
      const std::vector<vulkan_shader>& ray_gen_shaders,
      const std::vector<vulkan_shader>& hit_shaders,
      const std::vector<vulkan_shader>& miss_shaders);

  VkPipelineLayout get_vulkan_pipeline_layout() const;
  VkPipeline get_pipeline() const;

  VkPipelineBindPoint get_bind_point() const;
};
}  // namespace wunder

#endif  // WUNDER_VULKAN_PIPELINE_H
