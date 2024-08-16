#include "gla/vulkan/vulkan_pipeline.h"

#include <glad/vulkan.h>
namespace wunder {
class vulkan_shader;

unique_ptr<vulkan_pipeline> vulkan_pipeline::create_ray_tracing_pipeline(
    const std::vector<vulkan_shader>& ray_gen_shaders,
    const std::vector<vulkan_shader>& hit_shaders,
    const std::vector<vulkan_shader>& miss_shaders) {
  return nullptr;
}

VkPipelineLayout vulkan_pipeline::get_vulkan_pipeline_layout() const {
  return VK_NULL_HANDLE;
}

VkPipeline vulkan_pipeline::get_pipeline() const { return VK_NULL_HANDLE; }

VkPipelineBindPoint vulkan_pipeline::get_bind_point() const {
  return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
}

}  // namespace wunder
