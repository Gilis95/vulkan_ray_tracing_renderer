//
// Created by christian on 8/9/24.
//
#include "gla/vulkan/vulkan_context.h"

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "gla/renderer_capabilities .h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/vulkan.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_memory_allocator.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "window/window_factory.h"

namespace wunder {

vulkan_context::vulkan_context() = default;

vulkan_context::~vulkan_context() {
  // release them in reverse order
  if (m_renderer_capabilities.get()) {
    AssertLogUnless(m_renderer_capabilities.release());
  }
  if (m_resource_allocator.get()) {
    AssertLogUnless(m_resource_allocator.release());
  }
  if (m_logical_device.get()) {
    AssertLogUnless(m_logical_device.release());
  }
  if (m_physical_device.get()) {
    AssertLogUnless(m_physical_device.release());
  }
  if (m_vulkan.get()) {
    AssertLogUnless(m_vulkan.release());
  }
}

void vulkan_context::init(const wunder::renderer_properties &properties) {
  AssertReturnUnless(gladLoaderLoadVulkan(NULL, NULL, NULL));

  create_vulkan_instance(properties);
  // First figure out how many devices are in the system.
  select_physical_device();
  select_logical_device();

  AssertReturnUnless(gladLoaderLoadVulkan(
      m_vulkan->instance(), m_physical_device->get_vulkan_physical_device(),
      m_logical_device->get_vulkan_logical_device()));

  create_allocator();
}

void vulkan_context::create_vulkan_instance(
    const renderer_properties &properties) {
  m_vulkan = std::make_unique<vulkan>();
  m_vulkan->init(properties);
}

void vulkan_context::select_physical_device() {
  m_physical_device = std::make_unique<vulkan_physical_device>();
  m_physical_device->initialize();

  auto &properties = m_physical_device->get_properties();
  m_renderer_capabilities = std::make_unique<renderer_capabilities>();
  m_renderer_capabilities->vendor =
      vulkan_vendor_id_to_string(properties.vendorID);
  m_renderer_capabilities->device = properties.deviceName;
  m_renderer_capabilities->version = std::to_string(properties.driverVersion);
}

void vulkan_context::select_logical_device() {
  VkPhysicalDeviceFeatures enabled_features;
  memset(&enabled_features, 0, sizeof(VkPhysicalDeviceFeatures));
  enabled_features.samplerAnisotropy = true;
  enabled_features.wideLines = true;
  enabled_features.fillModeNonSolid = true;
  enabled_features.independentBlend = true;
  enabled_features.pipelineStatisticsQuery = true;
  enabled_features.shaderStorageImageReadWithoutFormat = true;

  m_logical_device = std::make_unique<vulkan_device>(enabled_features);
  m_logical_device->initialize();
}

void vulkan_context::create_allocator() {
  m_resource_allocator = make_unique<vulkan_memory_allocator>();
  m_resource_allocator->initialize();
}

const renderer_capabilities &vulkan_context::get_capabilities() const {
  static renderer_capabilities s_empty;
  return m_renderer_capabilities ? *m_renderer_capabilities : s_empty;
}

vulkan &vulkan_context::get_vulkan() { return *m_vulkan; }

vulkan_physical_device &vulkan_context::get_physical_device() {
  return *m_physical_device;
}

vulkan_device &vulkan_context::get_device() { return *m_logical_device; }

vulkan_memory_allocator &vulkan_context::get_resource_allocator() {
  return *m_resource_allocator;
}

}  // namespace wunder
