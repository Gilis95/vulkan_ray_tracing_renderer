//
// Created by christian on 8/14/24.
//

#include "gla/vulkan/vulkan_physical_device_types.h"

#include <cstring>

namespace wunder::vulkan {
features_11O_old::features_11O_old() {
  m_multiview.pNext = &m_t_16_bit_storage;
  m_t_16_bit_storage.pNext = &m_sampler_ycbcr_conversion;
  m_sampler_ycbcr_conversion.pNext = &m_protected_memory;
  m_protected_memory.pNext = &m_draw_parameters;
  m_draw_parameters.pNext = &m_variable_pointers;
  m_variable_pointers.pNext = nullptr;
}

void features_11O_old::read(
    const VkPhysicalDeviceVulkan11Features& features11) {
  m_multiview.multiview = features11.multiview;
  m_multiview.multiviewGeometryShader = features11.multiviewGeometryShader;
  m_multiview.multiviewTessellationShader =
      features11.multiviewTessellationShader;
  m_t_16_bit_storage.storageBuffer16BitAccess =
      features11.storageBuffer16BitAccess;
  m_t_16_bit_storage.storageInputOutput16 = features11.storageInputOutput16;
  m_t_16_bit_storage.storagePushConstant16 = features11.storagePushConstant16;
  m_t_16_bit_storage.uniformAndStorageBuffer16BitAccess =
      features11.uniformAndStorageBuffer16BitAccess;
  m_sampler_ycbcr_conversion.samplerYcbcrConversion =
      features11.samplerYcbcrConversion;
  m_protected_memory.protectedMemory = features11.protectedMemory;
  m_draw_parameters.shaderDrawParameters = features11.shaderDrawParameters;
  m_variable_pointers.variablePointers = features11.variablePointers;
  m_variable_pointers.variablePointersStorageBuffer =
      features11.variablePointersStorageBuffer;
}

void features_11O_old::write(VkPhysicalDeviceVulkan11Features& features11) {
  features11.multiview = m_multiview.multiview;
  features11.multiviewGeometryShader = m_multiview.multiviewGeometryShader;
  features11.multiviewTessellationShader =
      m_multiview.multiviewTessellationShader;
  features11.storageBuffer16BitAccess =
      m_t_16_bit_storage.storageBuffer16BitAccess;
  features11.storageInputOutput16 = m_t_16_bit_storage.storageInputOutput16;
  features11.storagePushConstant16 = m_t_16_bit_storage.storagePushConstant16;
  features11.uniformAndStorageBuffer16BitAccess =
      m_t_16_bit_storage.uniformAndStorageBuffer16BitAccess;
  features11.samplerYcbcrConversion =
      m_sampler_ycbcr_conversion.samplerYcbcrConversion;
  features11.protectedMemory = m_protected_memory.protectedMemory;
  features11.shaderDrawParameters = m_draw_parameters.shaderDrawParameters;
  features11.variablePointers = m_variable_pointers.variablePointers;
  features11.variablePointersStorageBuffer =
      m_variable_pointers.variablePointersStorageBuffer;
}

properties_11O_old::properties_11O_old() {
  m_maintenance_3.pNext = &m_device_id;
  m_device_id.pNext = &m_multiview;
  m_multiview.pNext = &m_protected_memory;
  m_protected_memory.pNext = &m_point_clipping;
  m_point_clipping.pNext = &m_subgroup_properties;
  m_subgroup_properties.pNext = nullptr;
}

void properties_11O_old::write(
    VkPhysicalDeviceVulkan11Properties& properties11) {
  memcpy(properties11.deviceLUID, m_device_id.deviceLUID,
         sizeof(properties11.deviceLUID));
  memcpy(properties11.deviceUUID, m_device_id.deviceUUID,
         sizeof(properties11.deviceUUID));
  memcpy(properties11.driverUUID, m_device_id.driverUUID,
         sizeof(properties11.driverUUID));
  properties11.deviceLUIDValid = m_device_id.deviceLUIDValid;
  properties11.deviceNodeMask = m_device_id.deviceNodeMask;
  properties11.subgroupSize = m_subgroup_properties.subgroupSize;
  properties11.subgroupSupportedStages = m_subgroup_properties.supportedStages;
  properties11.subgroupSupportedOperations =
      m_subgroup_properties.supportedOperations;
  properties11.subgroupQuadOperationsInAllStages =
      m_subgroup_properties.quadOperationsInAllStages;
  properties11.pointClippingBehavior = m_point_clipping.pointClippingBehavior;
  properties11.maxMultiviewViewCount = m_multiview.maxMultiviewViewCount;
  properties11.maxMultiviewInstanceIndex =
      m_multiview.maxMultiviewInstanceIndex;
  properties11.protectedNoFault = m_protected_memory.protectedNoFault;
  properties11.maxPerSetDescriptors = m_maintenance_3.maxPerSetDescriptors;
  properties11.maxMemoryAllocationSize =
      m_maintenance_3.maxMemoryAllocationSize;
}

void init_physical_info(physical_device_info& info,
                        VkPhysicalDevice physical_device, uint32_t version_major,
                        uint32_t version_minor) {
  vkGetPhysicalDeviceMemoryProperties(physical_device,
                                      &info.m_memory_properties);
  uint32_t count;
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);
  info.m_queue_properties.resize(count);
  vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count,
                                           info.m_queue_properties.data());

  static properties_11O_old properties_11_old;
  static features_11O_old features_11_old;

  if (version_major == 1 && version_minor == 1) {
    info.m_features_10.pNext = &features_11_old.m_multiview;
    info.m_properties_10.pNext = &properties_11_old.m_maintenance_3;
  } else if (version_major == 1 && version_minor >= 2) {
    info.m_features_10.pNext = &info.m_vulkan_11_features;
    info.m_vulkan_11_features.pNext = &info.m_vulkan_12_features;
    info.m_vulkan_12_features.pNext = nullptr;

    info.m_vulkan_12_properties.driverID = VK_DRIVER_ID_NVIDIA_PROPRIETARY;
    info.m_vulkan_12_properties.supportedDepthResolveModes =
        VK_RESOLVE_MODE_MAX_BIT;
    info.m_vulkan_12_properties.supportedStencilResolveModes =
        VK_RESOLVE_MODE_MAX_BIT;

    info.m_properties_10.pNext = &info.m_vulkan_11_properties;
    info.m_vulkan_11_properties.pNext = &info.m_vulkan_12_properties;
    info.m_vulkan_12_properties.pNext = nullptr;
  }

  if (version_major == 1 && version_minor >= 3) {
    info.m_vulkan_12_features.pNext = &info.m_vulkan_13_features;
    info.m_vulkan_13_features.pNext = nullptr;
    info.m_vulkan_12_properties.pNext = &info.m_vulkan_13_properties;
    info.m_vulkan_13_properties.pNext = nullptr;
  }

  if (version_major == 1 && version_minor == 1) {
    properties_11_old.write(info.m_vulkan_11_properties);
    features_11_old.write(info.m_vulkan_11_features);
  }

  // VERY IMPORTANT: we must initialize the list of features, that are going to
  // be used, before retrieving their support from the device, otherwise only
  // the first one will be initialized
  vkGetPhysicalDeviceFeatures2(physical_device, &info.m_features_10);
  vkGetPhysicalDeviceProperties2(physical_device, &info.m_properties_10);
}

}  // namespace wunder