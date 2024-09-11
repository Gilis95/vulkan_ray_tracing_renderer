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
#include "vulkan_extension_source.h"
#include "vulkan_physical_device_types.h"

namespace wunder {

class vulkan;
class vulkan_device;
struct physical_device_info;

class vulkan_physical_device : public vulkan_extension_source {
 private:
  friend vulkan_device;

 public:
  struct queue_family_indices {
    int32_t Graphics = -1;
    int32_t Compute = -1;
    int32_t Transfer = -1;
  };
  queue_family_indices get_queue_family_indices(int queueFlags) const;

 public:
  explicit vulkan_physical_device();
  ~vulkan_physical_device() override;

  void initialize();
 public:
  [[nodiscard]] bool is_extension_supported(
      const std::string& extensionName) const override;

 public:
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

  [[nodiscard]] VkFormat get_depth_format() const { return m_depth_format; }

  [[nodiscard]] const physical_device_info& get_device_info() const {
    return m_device_info;
  }

 private:
  [[nodiscard]] physical_device_info& mutable_device_info() {
    return m_device_info;
  }

 private:
  [[nodiscard]] VkResult select_gpu();
  void select_queue_family();

  [[nodiscard]] VkFormat find_depth_format() const;
 private:
  void load_supported_gpu_extensions();
 private:
  physical_device_info m_device_info;
  queue_family_indices m_queue_family_indices;
  std::unordered_set<std::string> m_supported_extensions;

  VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
  VkPhysicalDeviceProperties m_device_properties{};

  VkFormat m_depth_format = VK_FORMAT_UNDEFINED;

  std::vector<VkQueueFamilyProperties> m_queue_family_properties;
  std::vector<VkDeviceQueueCreateInfo> m_queue_create_infos;
};

}  // namespace wunder

#endif  // WUNDER_WUNDER_RENDERER_INCLUDE_GLA_VULKAN_VULKAN_DEVICE_H_
