//
// Created by christian on 8/14/24.
//

#ifndef WUNDER_VULKAN_PHYSICAL_DEVICE_TYPES_H
#define WUNDER_VULKAN_PHYSICAL_DEVICE_TYPES_H

#include <glad/vulkan.h>

#include <vector>
#include <cstdint>

namespace wunder::vulkan {
// This struct holds all core feature information for a physical device
struct physical_device_info {
 public:
  physical_device_info();

 public:
  VkPhysicalDeviceMemoryProperties m_memory_properties;
  std::vector<VkQueueFamilyProperties> m_queue_properties;

  VkPhysicalDeviceFeatures2 m_features_10;
  VkPhysicalDeviceVulkan11Features m_vulkan_11_features;
  VkPhysicalDeviceVulkan12Features m_vulkan_12_features;
  VkPhysicalDeviceVulkan13Features m_vulkan_13_features;

  VkPhysicalDeviceProperties2 m_properties_10;
  VkPhysicalDeviceVulkan11Properties m_vulkan_11_properties;
  VkPhysicalDeviceVulkan12Properties m_vulkan_12_properties;
  VkPhysicalDeviceVulkan13Properties m_vulkan_13_properties;
};

struct features_11O_old {
  features_11O_old();
  void read(const VkPhysicalDeviceVulkan11Features& features11);
  void write(VkPhysicalDeviceVulkan11Features& features11) const;

  VkPhysicalDeviceMultiviewFeatures m_multiview;
  VkPhysicalDevice16BitStorageFeatures m_t_16_bit_storage;
  VkPhysicalDeviceSamplerYcbcrConversionFeatures m_sampler_ycbcr_conversion;
  VkPhysicalDeviceProtectedMemoryFeatures m_protected_memory;
  VkPhysicalDeviceShaderDrawParameterFeatures m_draw_parameters;
  VkPhysicalDeviceVariablePointerFeatures m_variable_pointers;
};

struct properties_11O_old {
  properties_11O_old();
  void write(VkPhysicalDeviceVulkan11Properties& properties11) const;

  VkPhysicalDeviceMaintenance3Properties m_maintenance_3;
  VkPhysicalDeviceIDProperties m_device_id;
  VkPhysicalDeviceMultiviewProperties m_multiview;
  VkPhysicalDeviceProtectedMemoryProperties m_protected_memory;
  VkPhysicalDevicePointClippingProperties m_point_clipping;
  VkPhysicalDeviceSubgroupProperties m_subgroup_properties;
};

void init_physical_info(physical_device_info& info,
                        VkPhysicalDevice physical_device,
                        std::uint32_t version_major, std::uint32_t version_minor);

}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_PHYSICAL_DEVICE_TYPES_H
