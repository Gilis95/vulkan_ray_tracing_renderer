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
 public:
  rasterize_pipeline() noexcept;

 public:
  [[nodiscard]] VkPushConstantRange get_push_constant_range() const override;

  void initialize_pipeline(const vector_map<VkShaderStageFlagBits,
                                            std::vector<unique_ptr<shader>>>&
                               shaders_of_types);
 private:
  VkGraphicsPipelineCreateInfo m_pipeline_create_info;
  VkPipelineCache m_pipeline_cache;

  vulkan_rasterize_pipeline_state m_pipeline_state;
};
}  // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_RASTERIZE_PIPELINE_H
