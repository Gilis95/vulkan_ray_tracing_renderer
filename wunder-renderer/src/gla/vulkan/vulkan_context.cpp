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
#include "gla/vulkan/rasterize/vulkan_render_pass.h"
#include "gla/vulkan/rasterize/vulkan_swap_chain.h"
#include "window/window_factory.h"

namespace wunder::vulkan {
context::context() = default;

context::~context() {
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

void context::init(const wunder::renderer_properties &properties) {
  AssertReturnUnless(gladLoaderLoadVulkan(NULL, NULL, NULL));

  create_vulkan_instance(properties);
  // First figure out how many devices are in the system.
  select_physical_device();
  select_logical_device();

  AssertReturnUnless(gladLoaderLoadVulkan(
      m_vulkan->get_instance(), m_physical_device->get_vulkan_physical_device(),
      m_logical_device->get_vulkan_logical_device()));

  create_allocator();
  create_swap_chain(properties);
}

void context::create_vulkan_instance(const renderer_properties &properties) {
  m_vulkan = std::make_unique<instance>();
  m_vulkan->init(properties);
}

void context::select_physical_device() {
  m_physical_device = std::make_unique<physical_device>();
  m_physical_device->initialize();

  auto &properties = m_physical_device->get_properties();
  m_renderer_capabilities = std::make_unique<renderer_capabilities>();
  m_renderer_capabilities->vendor =
      vulkan_vendor_id_to_string(properties.vendorID);
  m_renderer_capabilities->device = properties.deviceName;
  m_renderer_capabilities->version = std::to_string(properties.driverVersion);
}

void context::select_logical_device() {
  VkPhysicalDeviceFeatures enabled_features;
  memset(&enabled_features, 0, sizeof(VkPhysicalDeviceFeatures));
  enabled_features.samplerAnisotropy = true;
  enabled_features.wideLines = true;
  enabled_features.fillModeNonSolid = true;
  enabled_features.independentBlend = true;
  enabled_features.pipelineStatisticsQuery = true;
  enabled_features.shaderStorageImageReadWithoutFormat = true;

  m_logical_device = std::make_unique<device>(enabled_features);
  m_logical_device->initialize();
}

void context::create_allocator() {
  m_resource_allocator = make_unique<memory_allocator>();
  m_resource_allocator->initialize();
}

void context::create_swap_chain(const renderer_properties &properties) {
  m_swap_chain =
      make_unique<swap_chain>(properties.m_width, properties.m_height);
  m_swap_chain->initialize();
}

const renderer_capabilities &context::get_capabilities() const {
  static renderer_capabilities s_empty;
  return m_renderer_capabilities ? *m_renderer_capabilities : s_empty;
}

instance &context::mutable_vulkan() { return *m_vulkan; }

physical_device &context::mutable_physical_device() { return *m_physical_device; }

device &context::mutable_device() { return *m_logical_device; }

swap_chain& context::mutable_swap_chain(){return *m_swap_chain;}


memory_allocator &context::mutable_resource_allocator() {
  return *m_resource_allocator;
}

}  // namespace wunder::vulkan
