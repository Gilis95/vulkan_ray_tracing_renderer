#ifndef WUNDER_VULKAN_MACROS_H
#define WUNDER_VULKAN_MACROS_H

#include <glad/vulkan.h>

#include <thread>

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"
#include "gla/renderer_capabilities .h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

#define VK_CHECK_RESULT(f)                                        \
  {                                                               \
    VkResult res = (f);                                           \
    wunder::vulkan::vulkan_check_result(res, __FILE__, __LINE__); \
  }

#define VK_CHECK_RESULT_RETURN(f)                                 \
  {                                                               \
    VkResult res = (f);                                           \
    wunder::vulkan::vulkan_check_result(res, __FILE__, __LINE__); \
    ReturnIf(res != VK_SUCCESS, res);                             \
  }

#define VK_CHECK_RESULT_CRASH(f)                                  \
  {                                                               \
    VkResult res = (f);                                           \
    wunder::vulkan::vulkan_check_result(res, __FILE__, __LINE__); \
    CrashIf(res != VK_SUCCESS, res);                              \
  }

namespace wunder::vulkan {

inline const char* vk_result_to_string(VkResult result) {
  switch (result) {
    case VK_SUCCESS:
      return "VK_SUCCESS";
    case VK_NOT_READY:
      return "VK_NOT_READY";
    case VK_TIMEOUT:
      return "VK_TIMEOUT";
    case VK_EVENT_SET:
      return "VK_EVENT_SET";
    case VK_EVENT_RESET:
      return "VK_EVENT_RESET";
    case VK_INCOMPLETE:
      return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED:
      return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST:
      return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED:
      return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT:
      return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT:
      return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS:
      return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
      return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL:
      return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_UNKNOWN:
      return "VK_ERROR_UNKNOWN";
    case VK_ERROR_OUT_OF_POOL_MEMORY:
      return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
      return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
    case VK_ERROR_FRAGMENTATION:
      return "VK_ERROR_FRAGMENTATION";
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
      return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
    case VK_ERROR_SURFACE_LOST_KHR:
      return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
      return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR:
      return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR:
      return "VK_ERROR_OUT_OF_DATE_KHR";
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
      return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
    case VK_ERROR_VALIDATION_FAILED_EXT:
      return "VK_ERROR_VALIDATION_FAILED_EXT";
    case VK_ERROR_INVALID_SHADER_NV:
      return "VK_ERROR_INVALID_SHADER_NV";
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
      return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
    case VK_ERROR_NOT_PERMITTED_EXT:
      return "VK_ERROR_NOT_PERMITTED_EXT";
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
      return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
    case VK_THREAD_IDLE_KHR:
      return "VK_THREAD_IDLE_KHR";
    case VK_THREAD_DONE_KHR:
      return "VK_THREAD_DONE_KHR";
    case VK_OPERATION_DEFERRED_KHR:
      return "VK_OPERATION_DEFERRED_KHR";
    case VK_OPERATION_NOT_DEFERRED_KHR:
      return "VK_OPERATION_NOT_DEFERRED_KHR";
    case VK_PIPELINE_COMPILE_REQUIRED_EXT:
      return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
    default:
      return nullptr;
  }
}

inline void dump_gpu_info() {
  auto& caps = layer_abstraction_factory::instance()
                   .get_vulkan_context()
                   .get_capabilities();
  WUNDER_TRACE_TAG("Renderer", "GPU Info:");
  WUNDER_TRACE_TAG("Renderer", "  vendor: {0}", caps.vendor);
  WUNDER_TRACE_TAG("Renderer", "  Device: {0}", caps.device);
  WUNDER_TRACE_TAG("Renderer", "  Version: {0}", caps.version);
}

inline void vulkan_check_result(VkResult result, const char* file, int line) {
  if (result != VK_SUCCESS) {
    WUNDER_ERROR_TAG("Renderer", "VkResult is '{0}' in {1}:{2}",
                     vk_result_to_string(result), file, line);
    if (result == VK_ERROR_DEVICE_LOST) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(3s);
      // Utils::RetrieveDiagnosticCheckpoints();
      dump_gpu_info();
    }
    AssertReturnUnless(result == VK_SUCCESS);
  }
}

[[maybe_unused]] static const char* vulkan_vendor_id_to_string(
    uint32_t vendorID) {
  switch (vendorID) {
    case 0x10DE:
      return "NVIDIA";
    case 0x1002:
      return "AMD";
    case 0x8086:
      return "INTEL";
    case 0x13B5:
      return "ARM";
    default:
      return "Unknown";
  }
}

static void set_debug_utils_object_name(VkDevice device,
                                        const VkObjectType objectType,
                                        const std::string& name,
                                        const void* handle) {
  ReturnUnless(vkSetDebugUtilsObjectNameEXT);

  VkDebugUtilsObjectNameInfoEXT nameInfo;
  nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
  nameInfo.objectType = objectType;
  nameInfo.pObjectName = name.c_str();
  nameInfo.objectHandle = (uint64_t)handle;
  nameInfo.pNext = VK_NULL_HANDLE;

  VK_CHECK_RESULT(vkSetDebugUtilsObjectNameEXT(device, &nameInfo));
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkBuffer handle) {
  set_debug_utils_object_name(device, VkObjectType::VK_OBJECT_TYPE_BUFFER, name,
                              handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkImage handle) {
  set_debug_utils_object_name(device, VK_OBJECT_TYPE_IMAGE, name, handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkImageView handle) {
  set_debug_utils_object_name(device, VK_OBJECT_TYPE_IMAGE_VIEW, name, handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkRenderPass handle) {
  set_debug_utils_object_name(device, VkObjectType::VK_OBJECT_TYPE_RENDER_PASS,
                              name, handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkSemaphore handle) {
  set_debug_utils_object_name(device, VkObjectType::VK_OBJECT_TYPE_SEMAPHORE,
                              name, handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkFramebuffer handle) {
  set_debug_utils_object_name(device, VkObjectType::VK_OBJECT_TYPE_FRAMEBUFFER,
                              name, handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkQueue handle) {
  set_debug_utils_object_name(device, VkObjectType::VK_OBJECT_TYPE_QUEUE, name,
                              handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkCommandPool handle) {
  set_debug_utils_object_name(device, VkObjectType::VK_OBJECT_TYPE_COMMAND_POOL, name,
                              handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkCommandBuffer handle) {
  set_debug_utils_object_name(device, VkObjectType::VK_OBJECT_TYPE_COMMAND_BUFFER, name,
                              handle);
}

inline static void set_debug_utils_object_name(VkDevice device,
                                               const std::string& name,
                                               VkDeviceMemory handle) {
  set_debug_utils_object_name(device, VkObjectType::VK_OBJECT_TYPE_DEVICE_MEMORY, name,
                              handle);
}

}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_MACROS_H
