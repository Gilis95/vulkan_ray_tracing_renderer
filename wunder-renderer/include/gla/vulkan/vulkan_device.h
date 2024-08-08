//
// Created by christian on 7/3/24.
//

#ifndef WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_DEVICE_H_
#define WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_DEVICE_H_
#pragma once

#include <glad/vulkan.h>

#include <string>
#include <unordered_set>
#include <vector>

#include "core/wunder_memory.h"

namespace wunder {

class vulkan;

class vulkan_physical_device {
 public:
  struct queue_family_indices {
    int32_t Graphics = -1;
    int32_t Compute = -1;
    int32_t Transfer = -1;
  };
  queue_family_indices get_queue_family_indices(int queueFlags) const;

 public:
  explicit vulkan_physical_device(shared_ptr<vulkan> vulkan);
  ~vulkan_physical_device();

  [[nodiscard]] bool is_extension_supported(
      const std::string& extensionName) const;
  uint32_t get_memory_type_index(uint32_t typeBits,
                                 VkMemoryPropertyFlags properties) const;

  [[nodiscard]] VkPhysicalDevice get_vulkan_physical_device() const {
    return m_physical_device;
  }
  [[nodiscard]] const queue_family_indices& get_queue_family_indices() const {
    return m_queue_family_indices;
  }

  [[nodiscard]] const VkPhysicalDeviceProperties& get_properties() const {
    return m_device_properties;
  }
  [[nodiscard]] const VkPhysicalDeviceLimits& get_limits() const {
    return m_device_properties.limits;
  }
  [[nodiscard]] const VkPhysicalDeviceMemoryProperties& get_memory_properties()
      const {
    return m_physical_device_memory_properties;
  }

  [[nodiscard]] VkFormat get_depth_format() const { return m_depth_format; }

 private:
  [[nodiscard]] VkResult select_gpu();
  void select_queue_family();

  [[nodiscard]] VkFormat find_depth_format() const;

 private:
  queue_family_indices m_queue_family_indices;

  VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
  VkPhysicalDeviceProperties m_device_properties{};
  VkPhysicalDeviceFeatures m_physical_device_features{};
  VkPhysicalDeviceMemoryProperties m_physical_device_memory_properties{};

  VkFormat m_depth_format = VK_FORMAT_UNDEFINED;

  wunder::shared_ptr<vulkan> m_vulkan;
  std::vector<VkQueueFamilyProperties> m_queue_family_properties;
  std::unordered_set<std::string> m_supported_extensions;
  std::vector<VkDeviceQueueCreateInfo> m_queue_create_infos;

  friend class vulkan_logical_device;
  void get_gpu_extensions();
};

}  // namespace wunder

#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_DEVICE_H_
