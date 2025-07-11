#ifndef WUNDER_VULKAN_RASTERIZE_PIPELINE_H
#define WUNDER_VULKAN_RASTERIZE_PIPELINE_H

#include <vector>

#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "gla/vulkan/rasterize/vulkan_rasterize_pipeline_state.h"
#include "gla/vulkan/vulkan_base_pipeline.h"

namespace wunder::vulkan {
class render_pass;
}

namespace wunder::vulkan {

class rasterize_pipeline : public base_pipeline {
 private:
  rasterize_pipeline() noexcept;

 public:
  static std::unique_ptr<rasterize_pipeline> create(
      const descriptor_set_manager& descriptor_set_manager,
      const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
          shaders);

 public:
  [[nodiscard]] VkPushConstantRange get_push_constant_range() const override;

 private:
  void initialize_pipeline(
      const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
          shaders_of_types);

  void initialize_rendering_info();

 private:
  VkGraphicsPipelineCreateInfo m_pipeline_create_info;
  VkPipelineRenderingCreateInfo m_pipeline_rendering_create_info;
  VkPipelineCache m_pipeline_cache;

  vulkan_rasterize_pipeline_state m_pipeline_state;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_RASTERIZE_PIPELINE_H
