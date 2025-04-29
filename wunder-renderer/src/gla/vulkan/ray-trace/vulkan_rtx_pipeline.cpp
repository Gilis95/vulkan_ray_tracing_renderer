#include "include/gla/vulkan/ray-trace/vulkan_rtx_pipeline.h"

#include <future>

#include "core/vector_map.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_shader.h"
#include "glad/vulkan.h"
#include "resources/shaders/host_device.h"

namespace wunder::vulkan {
class shader;

rtx_pipeline::rtx_pipeline()
    : base_pipeline(VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR),
      m_pipeline_create_info{} {
  m_pipeline_create_info.sType = {
      VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
}

std::unique_ptr<rtx_pipeline> rtx_pipeline::create(
    const descriptor_set_manager& descriptor_set_manager,
    const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
        shaders) {
  unique_ptr<rtx_pipeline> pipeline;
  pipeline.reset(new rtx_pipeline());

  pipeline->initialize_pipeline_layout(descriptor_set_manager);
  pipeline->initialize_pipeline(shaders);

  return pipeline;
}

VkPushConstantRange rtx_pipeline::get_push_constant_range() const {
  VkPushConstantRange push_constants{VK_SHADER_STAGE_RAYGEN_BIT_KHR |
                                         VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR |
                                         VK_SHADER_STAGE_MISS_BIT_KHR,
                                     0, sizeof(RtxState)};
  return push_constants;
}

void rtx_pipeline::initialize_pipeline(
    const vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>>&
        shaders_of_types) {
  auto& device = layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .mutable_device();

  vkDestroyPipeline(device.get_vulkan_logical_device(), m_vulkan_pipeline,
                    nullptr);

  create_shader_stage_create_info(shaders_of_types);
  create_shader_group_info();

  // --- Pipeline ---
  // Assemble the shader stages and recursion depth info into the ray tracing
  // pipeline
  m_pipeline_create_info.stageCount = static_cast<uint32_t>(
      m_shader_stage_create_infos.size());  // Stages are shaders
  m_pipeline_create_info.pStages = m_shader_stage_create_infos.data();

  m_pipeline_create_info.groupCount =
      static_cast<uint32_t>(m_shader_stage_groups.size());
  m_pipeline_create_info.pGroups = m_shader_stage_groups.data();

  m_pipeline_create_info.maxPipelineRayRecursionDepth = 2;  // Ray depth
  m_pipeline_create_info.layout = m_vulkan_pipeline_layout;

  // Create a deferred operation (compiling in parallel)
  bool use_deferred{true};
  VkDeferredOperationKHR deferred_op{VK_NULL_HANDLE};
  if (use_deferred) {
    VK_CHECK_RESULT(vkCreateDeferredOperationKHR(
        device.get_vulkan_logical_device(), nullptr, &deferred_op));
  }

  vkCreateRayTracingPipelinesKHR(device.get_vulkan_logical_device(),
                                 deferred_op, {}, 1, &m_pipeline_create_info,
                                 nullptr, &m_vulkan_pipeline);

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
    VK_CHECK_RESULT(vkGetDeferredOperationResultKHR(
        device.get_vulkan_logical_device(), deferred_op));

    vkDestroyDeferredOperationKHR(device.get_vulkan_logical_device(),
                                  deferred_op, nullptr);
  }
}

void rtx_pipeline::create_shader_group_info() {
  auto add_group = [&groups = m_shader_stage_groups](
                       VkRayTracingShaderGroupTypeKHR type,
                       uint32_t shader_offset) {
    VkRayTracingShaderGroupCreateInfoKHR group{};
    group.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    group.anyHitShader = VK_SHADER_UNUSED_KHR;
    group.closestHitShader = VK_SHADER_UNUSED_KHR;
    group.generalShader = VK_SHADER_UNUSED_KHR;
    group.intersectionShader = VK_SHADER_UNUSED_KHR;

    group.type = type;
    group.generalShader = shader_offset;
    groups.push_back(group);
  };

  auto add_hit_shader_group_info =
      [&groups = m_shader_stage_groups, &stages = m_shader_stage_create_infos](
          std::vector<VkPipelineShaderStageCreateInfo>::iterator&
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

        VkRayTracingShaderGroupCreateInfoKHR group{};
        group.sType =
            VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.generalShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;

        // closest hit shader
        group.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
        group.generalShader = VK_SHADER_UNUSED_KHR;

        if (closest_hit_begin != stages.end()) {
          group.closestHitShader = static_cast<uint32_t>(
              std::distance(stages.begin(), closest_hit_begin));
        }

        if (any_hit_begin != stages.end()) {
          group.anyHitShader = static_cast<uint32_t>(
              std::distance(stages.begin(), any_hit_begin));
        }

        groups.push_back(group);
        return true;
      };

  auto general_shader_type =
      VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR;

  for (size_t i = 0; i < m_shader_stage_create_infos.size(); ++i) {
    const auto& stage = m_shader_stage_create_infos[i];
    if (stage.stage & general_shader_type) {
      add_group(VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR,
                static_cast<uint32_t>(i));
    }
  }

  auto closest_hit_begin = m_shader_stage_create_infos.begin();
  auto any_hit_begin = m_shader_stage_create_infos.begin();

  while (add_hit_shader_group_info(closest_hit_begin, any_hit_begin)) {
    if (closest_hit_begin != m_shader_stage_create_infos.end()) {
      ++closest_hit_begin;
    }
    if (any_hit_begin != m_shader_stage_create_infos.end()) {
      ++any_hit_begin;
    }
  }
}

}  // namespace wunder::vulkan
