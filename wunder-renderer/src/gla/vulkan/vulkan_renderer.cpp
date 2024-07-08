#include "gla/vulkan/vulkan_renderer.h"

#include <glad/vulkan.h>

#include <optional>

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/vulkan_device.h"
#include "gla/vulkan/vulkan_logical_device.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "window/window_factory.h"

namespace {
struct QueueFamilyIndices {
  std::optional<uint32_t> m_graphics_family;

  bool is_complete() { return m_graphics_family.has_value(); }
};

GLADapiproc getVulkanFunction(const char *name) {}

const char *s_validation_layer_name = "VK_LAYER_KHRONOS_validation";


}  // namespace

namespace wunder {

vulkan_renderer::~vulkan_renderer() = default;

void vulkan_renderer::init_internal(const renderer_properties &properties) {
  AssertReturnIf(!gladLoaderLoadVulkan(NULL, NULL, NULL));

  //  gladLoadVulkan({}, getVulkanFunction);

  auto result = create_vulkan_instance(properties);
  AssertReturnIf(result != VK_SUCCESS);
  // First figure out how many devices are in the system.

  select_physical_device();
  select_logical_device();
}

VkResult vulkan_renderer::create_vulkan_instance(
    const renderer_properties &properties) {
  VkApplicationInfo app_info = {};
  std::memset(&app_info, 0, sizeof(app_info));

  // A generic application info structure
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "Wunder_Renderer";
  app_info.pEngineName = "Wunder_Renderer";
  app_info.apiVersion = VK_API_VERSION_1_3;
  app_info.pNext = nullptr;

  auto window_required_extensions =
      window_factory::get_instance().get_window().get_vulkan_extensions();

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

  instance_create_info.enabledExtensionCount =
      window_required_extensions.m_extensions_count;
  instance_create_info.ppEnabledExtensionNames =
      window_required_extensions.m_extensions;
  try_add_validation_layer(instance_create_info, properties);

  VkInstance *ptr = &m_vk_instance;
  AssertReturnIf(vkCreateInstance == nullptr, VkResult::VK_ERROR_DEVICE_LOST);
  auto ret = vkCreateInstance(&instance_create_info, nullptr, ptr);

  return ret;
}

VkResult vulkan_renderer::try_add_validation_layer(
    VkInstanceCreateInfo &instance_create_info,
    const renderer_properties &properties) const {
  ReturnUnless(properties.m_enable_validation, VkResult::VK_SUCCESS);
  // Check if this layer is available at instance level
  uint32_t instance_layer_count;
  vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr);
  std::vector<VkLayerProperties> instance_layer_properties(instance_layer_count);
  vkEnumerateInstanceLayerProperties(&instance_layer_count,
                                     instance_layer_properties.data());

  WUNDER_DEBUG("[Renderer] Vulkan Instance Layers:");
  for (const VkLayerProperties &layer : instance_layer_properties) {
    WUNDER_DEBUG("[Renderer] {0}", layer.layerName);

    if (strcmp(layer.layerName, s_validation_layer_name) == 0) {
      instance_create_info.ppEnabledLayerNames = &s_validation_layer_name;
      instance_create_info.enabledLayerCount = 1;

      return VkResult::VK_SUCCESS;
    }
  }

  WUNDER_WARN(
      "[Renderer] Validation layer VK_LAYER_KHRONOS_validation not present, "
      "validation is disabled");

  return VkResult::VK_SUCCESS;
}

void vulkan_renderer::select_physical_device() {
  m_physical_device = std::make_unique<vulkan_physical_device>(m_vk_instance);
}

void vulkan_renderer::select_logical_device() {
  VkPhysicalDeviceFeatures enabled_features;
  memset(&enabled_features, 0, sizeof(VkPhysicalDeviceFeatures));
  enabled_features.samplerAnisotropy = true;
  enabled_features.wideLines = true;
  enabled_features.fillModeNonSolid = true;
  enabled_features.independentBlend = true;
  enabled_features.pipelineStatisticsQuery = true;
  enabled_features.shaderStorageImageReadWithoutFormat = true;
  m_logical_device = std::make_unique<vulkan_logical_device>(*m_physical_device, enabled_features);
}

}  // namespace wunder
