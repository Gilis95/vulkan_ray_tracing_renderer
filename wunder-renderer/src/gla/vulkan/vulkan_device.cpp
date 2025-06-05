#include "gla/vulkan/vulkan_device.h"

#include <vector>

#include "gla/vulkan/vulkan.h"
#include "gla/vulkan/vulkan_command_pool.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "gla/vulkan/vulkan_shader_types.h"

namespace {
struct ExtensionHeader  // Helper struct to link extensions together
{
  VkStructureType sType;
  void* pNext;
};
}  // namespace

namespace wunder::vulkan {

////////////////////////////////////////////////////////////////////////////////////
// Vulkan Device
////////////////////////////////////////////////////////////////////////////////////

device::device(VkPhysicalDeviceFeatures /*enabled_features*/) {}

device::~device() = default;

void device::shutdown() {
  if (m_command_pool) {
    m_command_pool.reset();
  }

  VK_CHECK_RESULT(vkDeviceWaitIdle(m_logical_device));
  vkDestroyDevice(m_logical_device, nullptr);
}

void device::initialize() {
  auto& physical_device = layer_abstraction_factory::instance()
                              .get_vulkan_context()
                              .mutable_physical_device();
  create_extensions_list();
  create_logical_device();

  // Get a graphics queue from the device
  vkGetDeviceQueue(m_logical_device,
                   physical_device.m_queue_family_indices.Graphics, 0,
                   &m_graphics_queue);
  set_debug_utils_object_name(m_logical_device, "graphic queue", m_graphics_queue);

  vkGetDeviceQueue(m_logical_device,
                   physical_device.m_queue_family_indices.Compute, 0,
                   &m_compute_queue);
  set_debug_utils_object_name(m_logical_device, "compute queue", m_graphics_queue);

  m_command_pool = std::make_unique<command_pool>();
}

void device::create_extensions_list() {
  m_requested_extensions.push_back(
      {.m_name = VK_KHR_SWAPCHAIN_EXTENSION_NAME, .m_optional = false});
  m_requested_extensions.push_back(
      {.m_name = VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME,
       .m_optional = true});
  m_requested_extensions.push_back(
      {.m_name = VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME,
       .m_optional = true});
  m_requested_extensions.push_back(
      {.m_name = VK_EXT_DEBUG_MARKER_EXTENSION_NAME, .m_optional = true});

  m_requested_extensions.push_back(
      {.m_name = VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
       .m_optional = false});
  m_requested_extensions.push_back(
      {.m_name = VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
       .m_optional = false});
  m_requested_extensions.push_back(
      {.m_name = VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME, .m_optional = false});

  static VkPhysicalDeviceShaderClockFeaturesKHR clock_features_khr{};
  clock_features_khr.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
  m_requested_extensions.push_back(
      {.m_name = VK_KHR_SHADER_CLOCK_EXTENSION_NAME,
       .m_optional = false,
       .m_feature_struct = &clock_features_khr});

  // #VKRay: Activate the ray tracing extension
  static VkPhysicalDeviceAccelerationStructureFeaturesKHR
      acceleration_structure_features_khr{};
  acceleration_structure_features_khr.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
  m_requested_extensions.push_back(
      {.m_name = VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
       .m_optional = false,
       .m_feature_struct = &acceleration_structure_features_khr});

  static VkPhysicalDeviceRayTracingPipelineFeaturesKHR
      ray_tracing_pipeline_features_khr{};
  ray_tracing_pipeline_features_khr.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
  m_requested_extensions.push_back(
      {.m_name = VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
       .m_optional = false,
       .m_feature_struct = &ray_tracing_pipeline_features_khr});

  static VkPhysicalDeviceRayTracingValidationFeaturesNV validationFeatures = {
      .sType =
          VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_VALIDATION_FEATURES_NV,
      .pNext = VK_NULL_HANDLE,
      .rayTracingValidation = true,
  };
  m_requested_extensions.push_back(
      {.m_name = VK_NV_RAY_TRACING_VALIDATION_EXTENSION_NAME,
       .m_optional = false,
       .m_feature_struct = &validationFeatures});
}

void device::create_logical_device() {
  // If the device will be used for presenting to a display via a swapchain we
  // need to request the swapchain extension
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  auto& physical_device = vulkan_context.mutable_physical_device();
  auto& physical_device_info = physical_device.mutable_device_info();

  add_supported_extensions(physical_device, m_requested_extensions,
                           m_used_extensions);

  std::vector<char*> extension_names;
  extract_extensions_names(m_used_extensions, extension_names);

  std::vector<void*> used_features;
  extract_used_features_from_extensions(m_used_extensions, used_features);

  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount =
      static_cast<uint32_t>(physical_device.m_queue_create_infos.size());
  ;
  deviceCreateInfo.pQueueCreateInfos =
      physical_device.m_queue_create_infos.data();
  deviceCreateInfo.pEnabledFeatures = nullptr;

  if (!extension_names.empty()) {
    deviceCreateInfo.enabledExtensionCount = (uint32_t)extension_names.size();
    deviceCreateInfo.ppEnabledExtensionNames = extension_names.data();
  }

  append_used_device_features(physical_device_info, used_features,
                              deviceCreateInfo);

  VK_CHECK_RESULT(vkCreateDevice(physical_device.get_vulkan_physical_device(),
                                 &deviceCreateInfo, nullptr,
                                 &m_logical_device));
}

void device::append_used_device_features(
    physical_device_info& physical_device_info,
    const std::vector<void*>& used_features,
    VkDeviceCreateInfo& out_device_create_info) {  // use the features2 chain to
                                                   // append extensions
  context& vulkan_context =
      layer_abstraction_factory::instance().get_vulkan_context();
  const auto& physical_device = vulkan_context.mutable_physical_device();

  if (!used_features.empty()) {
    // build up chain of all used extension features
    for (size_t i = 0; i < used_features.size(); ++i) {
      auto* header = static_cast<ExtensionHeader*>(used_features[i]);
      header->pNext =
          i < used_features.size() - 1 ? used_features[i + 1] : nullptr;
    }

    auto* last_core_feature =
        reinterpret_cast<ExtensionHeader*>(&physical_device_info.m_features_10);
    while (last_core_feature->pNext != nullptr) {
      last_core_feature = static_cast<ExtensionHeader*>(last_core_feature->pNext);
    }

    // append required features to the end of physical device info
    last_core_feature->pNext =
        used_features[0];  // there's at least one element
                           // in the vector, so we're safe
  }

  out_device_create_info.pNext = &physical_device_info.m_features_10;
  vkGetPhysicalDeviceFeatures2(physical_device.get_vulkan_physical_device(),
                               &physical_device_info.m_features_10);
}

}  // namespace wunder::vulkan