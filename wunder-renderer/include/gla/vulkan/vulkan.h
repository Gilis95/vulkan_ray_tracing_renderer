//
// Created by christian on 8/8/24.
//

#ifndef WUNDER_VULKAN_H
#define WUNDER_VULKAN_H

#include <cstdint>
#include <string>
#include <vector>

#include "core/non_copyable.h"
#include "glad/vulkan.h"

namespace wunder {
struct renderer_properties;
struct vulkan_extensions;
class vulkan : public non_copyable{
 public:
  ~vulkan();

 public:
  void init(const renderer_properties& properties);

  [[nodiscard]] VkInstance instance() const { return m_vk_instance; }

 public:
  std::uint32_t get_api_major_version() const { return m_api_minor_version; }
  std::uint32_t get_api_minor_version() const { return m_api_minor_version; }
  std::uint32_t get_vulkan_version() const { return VK_MAKE_VERSION(m_api_major_version, m_api_minor_version, 0); }

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
  std::uint32_t m_api_major_version;
  std::uint32_t m_api_minor_version;
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_H
