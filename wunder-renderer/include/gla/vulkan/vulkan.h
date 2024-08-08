//
// Created by christian on 8/8/24.
//

#ifndef WUNDER_VULKAN_H
#define WUNDER_VULKAN_H

#include <string>
#include <vector>

#include "glad/vulkan.h"

namespace wunder {
struct renderer_properties;
struct vulkan_extensions;
class vulkan {
 public:
  ~vulkan();

 public:
  void init(const renderer_properties& properties);

  [[nodiscard]] VkInstance instance() const { return m_vk_instance; }
 private:
  static VkResult extract_supported_extensions(
      std::vector<VkExtensionProperties>& supported_extensions);
  static void log_supported_extensions(
      const std::vector<VkExtensionProperties>& supported_extensions);
  void try_add_debug_extension(
      vulkan_extensions& extensions,
      const std::vector<VkExtensionProperties>& supported_extensions,
      const renderer_properties& properties);

  static VkResult extract_supported_layers(
      std::vector<VkLayerProperties>& out_instance_layer_properties);
  static void log_supported_layers(
      const std::vector<VkLayerProperties>& supported_layers);
  void try_add_validation_layer(
      VkInstanceCreateInfo& instance_create_info,
      const std::vector<VkLayerProperties>& supported_layers,
      const renderer_properties& properties);

  void try_set_validation_message_callback();

 private:
  VkInstance m_vk_instance = VK_NULL_HANDLE;  // Vulkan library handle
  VkDebugUtilsMessengerEXT m_dbg_messenger = VK_NULL_HANDLE;

  std::vector<std::string> m_used_layers;
  std::vector<std::string> m_used_extensions;
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_H
