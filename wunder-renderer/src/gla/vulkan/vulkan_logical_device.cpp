#include "gla/vulkan/vulkan_logical_device.h"

#include <vector>

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_macros.h"

namespace wunder {

////////////////////////////////////////////////////////////////////////////////////
// Vulkan Device
////////////////////////////////////////////////////////////////////////////////////

vulkan_logical_device::vulkan_logical_device(
    shared_ptr<vulkan_physical_device> vulkan_physical_device,
    VkPhysicalDeviceFeatures enabled_features)
    : m_physical_device(vulkan_physical_device),
      m_enabled_features(enabled_features) {
  CrashUnless(m_physical_device);
  create_logical_device();

  // Get a graphics queue from the device
  vkGetDeviceQueue(m_logical_device,
                   m_physical_device->m_queue_family_indices.Graphics, 0,
                   &m_graphics_queue);
  vkGetDeviceQueue(m_logical_device,
                   m_physical_device->m_queue_family_indices.Compute, 0,
                   &m_compute_queue);

  m_command_pool = std::make_unique<vulkan_command_pool>(*this);
}

vulkan_logical_device::~vulkan_logical_device() { destroy(); }

void vulkan_logical_device::flush_command_buffer(
    VkCommandBuffer commandBuffer) {
  m_command_pool->flush_command_buffer(commandBuffer);
}

void vulkan_logical_device::flush_command_buffer(VkCommandBuffer commandBuffer,
                                                 VkQueue queue) {
  m_command_pool->flush_command_buffer(commandBuffer);
}

bool vulkan_logical_device::try_add_extension(
    std::vector<const char*>& device_extensions, const char* extension) const {
  if (m_physical_device->is_extension_supported(extension)) {
    device_extensions.push_back(extension);
    return true;
  }

  return false;
}

void vulkan_logical_device::create_logical_device() {
  std::vector<const char*> device_extensions;
  // If the device will be used for presenting to a display via a swapchain we
  // need to request the swapchain extension
  AssertReturnUnless(m_physical_device->is_extension_supported(
      VK_KHR_SWAPCHAIN_EXTENSION_NAME));

  device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  try_add_extension(device_extensions,
                    VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
  try_add_extension(device_extensions,
                    VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME);
  try_add_extension(device_extensions, VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount =
      static_cast<uint32_t>(m_physical_device->m_queue_create_infos.size());
  ;
  deviceCreateInfo.pQueueCreateInfos =
      m_physical_device->m_queue_create_infos.data();
  deviceCreateInfo.pEnabledFeatures = &m_enabled_features;

  if (!device_extensions.empty()) {
    deviceCreateInfo.enabledExtensionCount = (uint32_t)device_extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = device_extensions.data();
  }

  VK_CHECK_RESULT(vkCreateDevice(m_physical_device->get_vulkan_physical_device(),
                                 &deviceCreateInfo, nullptr,
                                 &m_logical_device));
}

void vulkan_logical_device::destroy() {
  VK_CHECK_RESULT(vkDeviceWaitIdle(m_logical_device));
  vkDestroyDevice(m_logical_device, nullptr);
}
}  // namespace wunder