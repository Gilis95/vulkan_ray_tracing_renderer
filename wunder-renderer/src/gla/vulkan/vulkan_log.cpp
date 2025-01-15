#include "gla/vulkan/vulkan_log.h"

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"

VKAPI_ATTR VkBool32 debug_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData) {
  // repeating nvprintfLevel to help with breakpoints : so we can selectively
  // break right after the print
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
    WUNDER_DEBUG_TAG("VULKAN", "[{0}]: {1}", callbackData->pMessageIdName,
                     callbackData->pMessage);
    return VK_FALSE;
  }

  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    WUNDER_INFO_TAG("VULKAN", "[{0}]: {1}", callbackData->pMessageIdName,
                    callbackData->pMessage);
    return VK_FALSE;
  }

  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    WUNDER_WARN_TAG("VULKAN", "[{0}]: {1}", callbackData->pMessageIdName,
                    callbackData->pMessage);
    return VK_FALSE;
  }

  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    WUNDER_ERROR_TAG("VULKAN", "[{0}]: {1}", callbackData->pMessageIdName,
                     callbackData->pMessage);

    CRASH;

    return VK_FALSE;
  }

  if (messageType & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
    WUNDER_INFO_TAG("VULKAN GENERAL", "[{0}]: {1}",
                    callbackData->pMessageIdName, callbackData->pMessage);
    return VK_FALSE;
  }

  WUNDER_INFO_TAG("VULKAN GENERAL", "[{0}]: {1}",
                  callbackData->pMessageIdName, callbackData->pMessage);
  // Don't bail out, but keep going.
  return VK_FALSE;
}