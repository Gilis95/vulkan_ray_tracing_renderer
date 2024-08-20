#include "gla/vulkan/vulkan_pipeline.h"

#include <glad/vulkan.h>

#include <future>

#include "core/vector_map.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_shader.h"
#include "resources/shaders/host_device.h"

namespace wunder {
class vulkan_shader;

VkPipelineLayout vulkan_pipeline::get_vulkan_pipeline_layout() const {
  return VK_NULL_HANDLE;
}

void vulkan_pipeline::create_pipeline_layout(
    const vulkan_shader& descriptor_declaring_shader) {
  auto& device = vulkan_layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .get_device();

  const auto& descriptor_sets_layout =
      descriptor_declaring_shader.get_descriptor_set_layout();

  vkDestroyPipelineLayout(device.get_vulkan_logical_device(),
                          m_vulkan_pipeline_layout, nullptr);

  //TODO:: This must be comming from shader reflect data!!!
  VkPushConstantRange push_constants{VK_SHADER_STAGE_RAYGEN_BIT_KHR |
                                       VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR |
                                       VK_SHADER_STAGE_MISS_BIT_KHR,
                                   0, sizeof(RtxState)};

  VkPipelineLayoutCreateInfo pipeline_layout_create_info{
      VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
  pipeline_layout_create_info.pushConstantRangeCount = 1;
  pipeline_layout_create_info.pPushConstantRanges = &push_constants;
  pipeline_layout_create_info.setLayoutCount =
      static_cast<uint32_t>(descriptor_sets_layout.size());
  pipeline_layout_create_info.pSetLayouts = descriptor_sets_layout.data();
  vkCreatePipelineLayout(device.get_vulkan_logical_device(),
                         &pipeline_layout_create_info, nullptr,
                         &m_vulkan_pipeline_layout);
}

void vulkan_pipeline::create_pipeline(
    const vector_map<VkShaderStageFlagBits,
                     std::vector<unique_ptr<vulkan_shader>>>&
        shaders_of_types) {
  auto& device = vulkan_layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .get_device();

  vkDestroyPipeline(device.get_vulkan_logical_device(), m_vulkan_pipeline,
                    nullptr);

  std::vector<VkPipelineShaderStageCreateInfo> stages =
      get_shader_stage_create_info(shaders_of_types);
  std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups =
      get_shader_group_create_info(stages);

  // --- Pipeline ---
  // Assemble the shader stages and recursion depth info into the ray tracing
  // pipeline
  VkRayTracingPipelineCreateInfoKHR ray_tracing_pipeline_create_info_khr{
      VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
  ray_tracing_pipeline_create_info_khr.stageCount =
      static_cast<uint32_t>(stages.size());  // Stages are shaders
  ray_tracing_pipeline_create_info_khr.pStages = stages.data();

  ray_tracing_pipeline_create_info_khr.groupCount = static_cast<uint32_t>(groups.size());
  ray_tracing_pipeline_create_info_khr.pGroups = groups.data();

  ray_tracing_pipeline_create_info_khr.maxPipelineRayRecursionDepth = 2;  // Ray depth
  ray_tracing_pipeline_create_info_khr.layout = m_vulkan_pipeline_layout;

  // Create a deferred operation (compiling in parallel)
  bool use_deferred{true};
  VkDeferredOperationKHR deferred_op{VK_NULL_HANDLE};
  if (use_deferred) {
    VK_CHECK_RESULT(vkCreateDeferredOperationKHR(device.get_vulkan_logical_device(),
                                          nullptr, &deferred_op));
  }

  vkCreateRayTracingPipelinesKHR(device.get_vulkan_logical_device(), deferred_op,
                                 {}, 1, &ray_tracing_pipeline_create_info_khr, nullptr,
                                 &m_vulkan_pipeline);

  if (use_deferred) {
    // Query the maximum amount of concurrency and clamp to the desired maximum
    uint32_t max_threads{8};
    uint32_t launches =
        std::min(vkGetDeferredOperationMaxConcurrencyKHR(
                     device.get_vulkan_logical_device(), deferred_op),
                 max_threads);

    std::vector<std::future<void>> joins;
    for (uint32_t i = 0; i < launches; i++) {
      joins.emplace_back(std::async(
          std::launch::async,
          [device = device.get_vulkan_logical_device(), deferred_op]() {
            // A return of VK_THREAD_IDLE_KHR should queue another job
            vkDeferredOperationJoinKHR(device, deferred_op);
          }));
    }

    for (auto& f : joins) {
      f.get();
    }

    // deferred operation is now complete.  'result' indicates success or
    // failure
    VK_CHECK_RESULT(vkGetDeferredOperationResultKHR(device.get_vulkan_logical_device(), deferred_op));

    vkDestroyDeferredOperationKHR(device.get_vulkan_logical_device(),
                                  deferred_op, nullptr);
  }
}

std::vector<VkPipelineShaderStageCreateInfo>
vulkan_pipeline::get_shader_stage_create_info(
    const vector_map<VkShaderStageFlagBits,
                     std::vector<unique_ptr<vulkan_shader>>>&
        shaders_of_types) {
  std::vector<VkPipelineShaderStageCreateInfo> stages;

  for (auto& [shader_type, shaders] : shaders_of_types) {
    for (auto& shader : shaders) {
      stages.push_back(shader->get_shader_stage_info());
    }
  }

  return stages;
}

std::vector<VkRayTracingShaderGroupCreateInfoKHR>
vulkan_pipeline::get_shader_group_create_info(
    std::vector<VkPipelineShaderStageCreateInfo>& stages) const {
  std::vector<VkRayTracingShaderGroupCreateInfoKHR> groups;
  auto add_group = [&groups](VkRayTracingShaderGroupTypeKHR type,
                             uint32_t shader_offset) {
    VkRayTracingShaderGroupCreateInfoKHR group{
        VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR};
    group.anyHitShader = VK_SHADER_UNUSED_KHR;
    group.closestHitShader = VK_SHADER_UNUSED_KHR;
    group.generalShader = VK_SHADER_UNUSED_KHR;
    group.intersectionShader = VK_SHADER_UNUSED_KHR;

    group.type = type;
    group.generalShader = shader_offset;
    groups.push_back(group);
  };

  auto add_hit_shader_group_info =
      [&groups, &stages](std::vector<VkPipelineShaderStageCreateInfo>::iterator&
                             closest_hit_begin,
                         std::vector<VkPipelineShaderStageCreateInfo>::iterator&
                             any_hit_begin) {
        closest_hit_begin = std::find_if(
            closest_hit_begin, stages.end(),
            [](const VkPipelineShaderStageCreateInfo& stage_create_info) {
              return stage_create_info.stage &
                     VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
            });

        any_hit_begin = std::find_if(
            any_hit_begin, stages.end(),
            [](const VkPipelineShaderStageCreateInfo& stage_create_info) {
              return stage_create_info.stage & VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
            });

        ReturnIf(
            closest_hit_begin == stages.end() && any_hit_begin == stages.end(),
            false);

        VkRayTracingShaderGroupCreateInfoKHR group{
            VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR};
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.generalShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;

        // closest hit shader
        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        group.generalShader = VK_SHADER_UNUSED_KHR;

        if (closest_hit_begin != stages.end()) {
          group.closestHitShader =
              std::distance(stages.begin(), closest_hit_begin);
        }

        if (any_hit_begin != stages.end()) {
          group.anyHitShader = std::distance(stages.begin(), any_hit_begin);
        }

        groups.push_back(group);
        return true;
      };

  auto general_shader_type =
      VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;

  for (size_t i = 0; i < stages.size(); ++i) {
    const auto& stage = stages[i];
    if (stage.stage & general_shader_type) {
      add_group(VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR, i);
    }
  }

  auto closest_hit_begin = stages.begin();
  auto any_hit_begin = stages.begin();

  while (add_hit_shader_group_info(closest_hit_begin, any_hit_begin)) {
    if (closest_hit_begin != stages.end()) {
      ++closest_hit_begin;
    }
    if (any_hit_begin != stages.end()) {
      ++any_hit_begin;
    }
  }

  return groups;
}

VkPipeline vulkan_pipeline::get_pipeline() const { return VK_NULL_HANDLE; }

VkPipelineBindPoint vulkan_pipeline::get_bind_point() const {
  return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
}
}  // namespace wunder
