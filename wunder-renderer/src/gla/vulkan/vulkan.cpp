#include "gla/vulkan/vulkan.h"

#include <cstring>

#include "core/wunder_macros.h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/vulkan_log.h"
#include "gla/vulkan/vulkan_macros.h"
#include "window/window_factory.h"

namespace {
const char *s_validation_layer_name = "VK_LAYER_KHRONOS_validation";
}  // namespace

namespace wunder {

vulkan::~vulkan() = default;

void vulkan::init(const renderer_properties &properties) {
  VkApplicationInfo app_info = {};
  std::memset(&app_info, 0, sizeof(app_info));

  // A generic application info structure
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Wunder_Renderer";
  app_info.pEngineName = "Wunder_Renderer";
  app_info.apiVersion = VK_API_VERSION_1_3;
  app_info.pNext = nullptr;

  VkValidationFeatureEnableEXT enables[] = {
      VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT};
  VkValidationFeaturesEXT features = {};
  std::memset(&features, 0, sizeof(features));

  features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
  features.enabledValidationFeatureCount = 1;
  features.pEnabledValidationFeatures = enables;

  // Create the instance.
  VkInstanceCreateInfo instance_create_info = {};
  std::memset(&instance_create_info, 0, sizeof(instance_create_info));

  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pNext = nullptr;  // &features;
  instance_create_info.pApplicationInfo = &app_info;
  instance_create_info.enabledLayerCount = 0;

  vulkan_extensions vulkan_extensions;
  window_factory::get_instance().get_window().fill_vulkan_extensions(
      vulkan_extensions);

  std::vector<VkExtensionProperties> supported_extensions;
  extract_supported_extensions(supported_extensions);
  log_supported_extensions(supported_extensions);
  try_add_debug_extension(vulkan_extensions, supported_extensions, properties);

  instance_create_info.enabledExtensionCount =
      vulkan_extensions.m_extensions.size();
  instance_create_info.ppEnabledExtensionNames =
      vulkan_extensions.m_extensions.data();

  std::vector<VkLayerProperties> supported_layers;
  extract_supported_layers(supported_layers);
  log_supported_layers(supported_layers);
  try_add_validation_layer(instance_create_info, supported_layers, properties);

  VkInstance *ptr = &m_vk_instance;
  CrashIf(vkCreateInstance == nullptr);
  VK_CHECK_RESULT_CRASH(vkCreateInstance(&instance_create_info, nullptr, ptr));

  try_set_validation_message_callback();
}

VkResult vulkan::extract_supported_extensions(
    std::vector<VkExtensionProperties> &supported_extensions) {
  uint32_t count;

  VK_CHECK_RESULT_RETURN(
      vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr));
  supported_extensions.resize(count);
  return vkEnumerateInstanceExtensionProperties(nullptr, &count,
                                                supported_extensions.data());
}

void vulkan::log_supported_extensions(
    const std::vector<VkExtensionProperties> &supported_extensions) {
  WUNDER_TRACE_TAG("Renderer", "Vulkan Instance Extensions. Count {0}:",
                   supported_extensions.size());

  for (const VkExtensionProperties &layer : supported_extensions) {
    WUNDER_TRACE_TAG("Renderer", "{0}", layer.extensionName);
  }
  WUNDER_TRACE_TAG("Renderer", "------------------------------------");
}

void vulkan::try_add_debug_extension(
    vulkan_extensions &extensions,
    const std::vector<VkExtensionProperties> &supported_extensions,
    const renderer_properties &properties) {
  ReturnUnless(properties.m_enable_validation, );
  // Check if this layer is available at instance level

  if (std::none_of(supported_extensions.begin(), supported_extensions.end(),
                   [](const VkExtensionProperties &extension_properties) {
                     return strcmp(extension_properties.extensionName,
                                   VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
                   })) {
    WUNDER_WARN_TAG("Renderer",
                    "Validation layer VK_LAYER_KHRONOS_validation not present, "
                    "validation is disabled");

    return;
  }

  m_used_extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  extensions.m_extensions.reserve(1);
  extensions.m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

VkResult vulkan::extract_supported_layers(
    std::vector<VkLayerProperties> &out_instance_layer_properties) {
  uint32_t instance_layer_count;

  VK_CHECK_RESULT_RETURN(
      vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));
  out_instance_layer_properties.resize(instance_layer_count);
  return vkEnumerateInstanceLayerProperties(
      &instance_layer_count, out_instance_layer_properties.data());
}

void vulkan::log_supported_layers(
    const std::vector<VkLayerProperties> &supported_layers) {
  WUNDER_TRACE_TAG("Renderer", "Vulkan Instance Layers. Count {0}:",
                   supported_layers.size());

  for (const VkLayerProperties &layer : supported_layers) {
    WUNDER_TRACE_TAG("Renderer", "{0}", layer.layerName);
  }
  WUNDER_TRACE_TAG("Renderer", "------------------------------------");
}

void vulkan::try_add_validation_layer(
    VkInstanceCreateInfo &instance_create_info,
    const std::vector<VkLayerProperties> &supported_layers,
    const renderer_properties &properties) {
  ReturnUnless(properties.m_enable_validation, );
  // Check if this layer is available at instance level

  if (std::none_of(supported_layers.begin(), supported_layers.end(),
                   [](const VkLayerProperties &layer) {
                     return strcmp(layer.layerName, s_validation_layer_name);
                   })) {
    WUNDER_WARN_TAG("Renderer",
                    "Validation layer VK_LAYER_KHRONOS_validation not present, "
                    "validation is disabled");

    return;
  }

  m_used_layers.emplace_back(s_validation_layer_name);
  instance_create_info.ppEnabledLayerNames = &s_validation_layer_name;
  instance_create_info.enabledLayerCount = 1;
}

void vulkan::try_set_validation_message_callback() {
  // Create a Debug Utils Messenger that will trigger our callback for any
  // warning or error.
  auto m_createDebugUtilsMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
          m_vk_instance, "vkCreateDebugUtilsMessengerEXT");

  AssertReturnIf(m_createDebugUtilsMessengerEXT == nullptr)

      VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info{
          VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
  dbg_messenger_create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT       // For debug printf
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT  // GPU info, bug
      | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;   // Invalid usage
  dbg_messenger_create_info.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT         // Other
      | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT    // Violation of spec
      | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;  // Non-optimal use
  dbg_messenger_create_info.pfnUserCallback = debug_messenger_callback;
  dbg_messenger_create_info.pUserData = this;
  VK_CHECK_RESULT(m_createDebugUtilsMessengerEXT(
      m_vk_instance, &dbg_messenger_create_info, nullptr, &m_dbg_messenger));
}

}  // namespace wunder
