//
// Created by christian on 8/14/24.
//

#ifndef WUNDER_VULKAN_PHYSICAL_DEVICE_TYPES_H
#define WUNDER_VULKAN_PHYSICAL_DEVICE_TYPES_H

#include <glad/vulkan.h>

#include <vector>

namespace wunder::vulkan {
// This struct holds all core feature information for a physical device
struct physical_device_info {
  VkPhysicalDeviceMemoryProperties m_memory_properties{};
  std::vector<VkQueueFamilyProperties> m_queue_properties;

  VkPhysicalDeviceFeatures2 m_features_10{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2};
  VkPhysicalDeviceVulkan11Features m_vulkan_11_features{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES};
  VkPhysicalDeviceVulkan12Features m_vulkan_12_features{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
  VkPhysicalDeviceVulkan13Features m_vulkan_13_features{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};

  VkPhysicalDeviceProperties2 m_properties_10{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
  VkPhysicalDeviceVulkan11Properties m_vulkan_11_properties{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES};
  VkPhysicalDeviceVulkan12Properties m_vulkan_12_properties{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES};
  VkPhysicalDeviceVulkan13Properties m_vulkan_13_properties{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES};
};

struct features_11O_old {
  features_11O_old();
  void read(const VkPhysicalDeviceVulkan11Features& features11);
  void write(VkPhysicalDeviceVulkan11Features& features11);

  VkPhysicalDeviceMultiviewFeatures m_multiview{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES};
  VkPhysicalDevice16BitStorageFeatures m_t_16_bit_storage{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES};
  VkPhysicalDeviceSamplerYcbcrConversionFeatures m_sampler_ycbcr_conversion{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SAMPLER_YCBCR_CONVERSION_FEATURES};
  VkPhysicalDeviceProtectedMemoryFeatures m_protected_memory{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_FEATURES};
  VkPhysicalDeviceShaderDrawParameterFeatures m_draw_parameters{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES};
  VkPhysicalDeviceVariablePointerFeatures m_variable_pointers{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VARIABLE_POINTER_FEATURES};
};

struct properties_11O_old {
  properties_11O_old();
  void write(VkPhysicalDeviceVulkan11Properties& properties11);

  VkPhysicalDeviceMaintenance3Properties m_maintenance_3{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES};
  VkPhysicalDeviceIDProperties m_device_id{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES};
  VkPhysicalDeviceMultiviewProperties m_multiview{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES};
  VkPhysicalDeviceProtectedMemoryProperties m_protected_memory{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROTECTED_MEMORY_PROPERTIES};
  VkPhysicalDevicePointClippingProperties m_point_clipping{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_POINT_CLIPPING_PROPERTIES};
  VkPhysicalDeviceSubgroupProperties m_subgroup_properties{
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES};
};

void init_physical_info(physical_device_info& info,
                        VkPhysicalDevice physical_device,
                        uint32_t version_major, uint32_t version_minor);

}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_PHYSICAL_DEVICE_TYPES_H
