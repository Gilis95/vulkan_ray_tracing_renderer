
#ifndef WUNDER_VULKAN_LOG_H
#define WUNDER_VULKAN_LOG_H

#include <glad/vulkan.h>

VKAPI_ATTR VkBool32 debug_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *callbackData, void *userData);

#endif  // WUNDER_VULKAN_LOG_H
