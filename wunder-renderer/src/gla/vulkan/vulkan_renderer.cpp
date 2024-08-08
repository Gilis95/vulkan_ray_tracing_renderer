#include "gla/vulkan/vulkan_renderer.h"

#include <glad/vulkan.h>

#include <optional>

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "gla/renderer_capabilities .h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/vulkan.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_log.h"
#include "gla/vulkan/vulkan_logical_device.h"
#include "gla/vulkan/vulkan_macros.h"
#include "window/window_factory.h"

namespace wunder {

vulkan_renderer::~vulkan_renderer(){};

void vulkan_renderer::init_internal(const renderer_properties &properties) {
  AssertReturnUnless(gladLoaderLoadVulkan(NULL, NULL, NULL));

  create_vulkan_instance(properties);
  // First figure out how many devices are in the system.
  select_physical_device();
  select_logical_device();

  AssertReturnUnless(gladLoaderLoadVulkan(
      m_vulkan->instance(), m_physical_device->get_vulkan_physical_device(),
      m_logical_device->get_vulkan_logical_device()));
}

void vulkan_renderer::create_vulkan_instance(
    const renderer_properties &properties) {
  m_vulkan = std::make_shared<vulkan>();
  m_vulkan->init(properties);
}

void vulkan_renderer::select_physical_device() {
  m_physical_device = std::make_shared<vulkan_physical_device>(m_vulkan);

  AssertReturnUnless(m_physical_device);

  auto &properties = m_physical_device->get_properties();
  m_renderer_capabilities = std::make_unique<renderer_capabilities>();
  m_renderer_capabilities->vendor =
      vulkan_vendor_id_to_string(properties.vendorID);
  m_renderer_capabilities->device = properties.deviceName;
  m_renderer_capabilities->version = std::to_string(properties.driverVersion);
}

void vulkan_renderer::select_logical_device() {
  VkPhysicalDeviceFeatures enabled_features;
  memset(&enabled_features, 0, sizeof(VkPhysicalDeviceFeatures));
  enabled_features.samplerAnisotropy = true;
  enabled_features.wideLines = true;
  enabled_features.fillModeNonSolid = true;
  enabled_features.independentBlend = true;
  enabled_features.pipelineStatisticsQuery = true;
  enabled_features.shaderStorageImageReadWithoutFormat = true;
  m_logical_device = std::make_unique<vulkan_logical_device>(m_physical_device,
                                                             enabled_features);
}

void vulkan_renderer::update(int dt) /*override*/
{
  auto graphic_command_buffer = m_logical_device->get_graphics_queue();
  //  vkCmdBindPipeline(graphic_command_buffer,
  //  VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_rtPipeline);
  //  vkCmdBindDescriptorSets(
  //      graphic_command_buffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
  //      m_rtPipelineLayout, 0,
  //                          static_cast<uint32_t>(descSets.size()),
  //                          descSets.data(), 0, nullptr);
  //  vkCmdPushConstants(graphic_command_buffer, m_rtPipelineLayout,
  //                     VK_SHADER_STAGE_RAYGEN_BIT_KHR |
  //                     VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR |
  //                     VK_SHADER_STAGE_MISS_BIT_KHR, 0, sizeof(RtxState),
  //                     &m_state);
  //
  //
  //  auto& regions = m_sbtWrapper.getRegions();
  //  vkCmdTraceRaysKHR(graphic_command_buffer, &regions[0], &regions[1],
  //  &regions[2], &regions[3], size.width, size.height, 1);
}

renderer_capabilities &vulkan_renderer::get_capabilities() /*override*/
{
  static renderer_capabilities s_empty;
  return m_renderer_capabilities ? *m_renderer_capabilities : s_empty;
}

}  // namespace wunder