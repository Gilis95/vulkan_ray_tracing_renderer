#include "gla/vulkan/vulkan_renderer.h"

#include <glad/vulkan.h>

#include <optional>

#include "core/wunder_macros.h"
#include "window/window_factory.h"

namespace {
struct QueueFamilyIndices {
  std::optional<uint32_t> m_graphics_family;

  bool is_complete() { return m_graphics_family.has_value(); }
};
}  // namespace

namespace wunder {

vulkan_renderer::~vulkan_renderer() { vkDestroyDevice(m_device, nullptr); }

void vulkan_renderer::init_internal(const renderer_properties &properties) {
  auto result = create_vulkan_instance();
  AssertReturnIf(result != VK_SUCCESS);
  // First figure out how many devices are in the system.

  result = select_gpu();
  AssertReturnIf(result != VK_SUCCESS);

  result = select_queue_family();
  AssertReturnUnless(result == VK_SUCCESS)

      result = create_vulkan_logical_device();
  AssertReturnUnless(result == VK_SUCCESS)

      vkGetDeviceQueue(m_device, m_selected_queue_family, 0, &m_queue);

  result = create_descriptor_pool();
}

VkResult vulkan_renderer::create_vulkan_instance() {
  VkApplicationInfo app_info = {};
  VkInstanceCreateInfo instance_create_info = {};

  // A generic application info structure
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName = "renderer\0";
  app_info.applicationVersion = 1;
  app_info.apiVersion = VK_API_VERSION_1_3;

  // Create the instance.
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pApplicationInfo = &app_info;

  auto window_required_extensions =
      window_factory::get_instance().get_window().get_vulkan_extensions();

  instance_create_info.enabledExtensionCount =
      window_required_extensions.m_extensions_count;
  instance_create_info.ppEnabledExtensionNames =
      window_required_extensions.m_extensions;
  instance_create_info.enabledLayerCount = 0;

  return vkCreateInstance(&instance_create_info, nullptr, &m_vk_instance);
}

VkResult vulkan_renderer::select_gpu() {
  uint32_t physical_device_count = 0;
  auto result = vkEnumeratePhysicalDevices(m_vk_instance,
                                           &physical_device_count, nullptr);
  AssertReturnIf(result != VK_SUCCESS, result);

  AssertReturnUnless(0 < physical_device_count, VK_ERROR_UNKNOWN);

  // Size the device array appropriately and get the physical
  // device handles.
  std::vector<VkPhysicalDevice> physical_devices(
      physical_device_count);  // list of available gpu`s
  vkEnumeratePhysicalDevices(m_vk_instance, &physical_device_count,
                             &physical_devices[0]);

  for (auto physical_device : physical_devices) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    ContinueUnless(properties.deviceType ==
                   VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
    m_physical_device = physical_device;
  }

  // fallback to first gpu
  if (m_physical_device == nullptr) {
    m_physical_device = physical_devices[0];
  }

  return VK_SUCCESS;
}

VkResult vulkan_renderer::select_queue_family() {
  uint32_t queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device,
                                           &queue_family_count, nullptr);

  std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(
      m_physical_device, &queue_family_count, queue_families.data());

  m_selected_queue_family = 0;
  for (const auto &queue_family : queue_families) {
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      break;
    }

    ++m_selected_queue_family;
  }
}

VkResult vulkan_renderer::create_vulkan_logical_device() {
  int device_extension_count = 1;
  const char *device_extensions[] = {"VK_KHR_swapchain"};
  VkDeviceQueueCreateInfo queues_info[1] = {};
  auto &queue_info = queues_info[0];
  queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info.queueFamilyIndex = m_selected_queue_family;
  queue_info.queueCount = 1;
  float queuePriority = 1.0f;
  queue_info.pQueuePriorities = &queuePriority;

  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount =
      sizeof(queues_info) / sizeof(queues_info[0]);
  create_info.pQueueCreateInfos = queues_info;
  create_info.enabledExtensionCount = device_extension_count;
  create_info.ppEnabledExtensionNames = device_extensions;

  return vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device);
}

VkResult vulkan_renderer::create_descriptor_pool() {
  //
  //        VkDescriptorPoolSize pool_sizes[] =
  //                {
  //                        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
  //                        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
  //                };
  //
  //        VkDescriptorPoolCreateInfo pool_info = {};
  //        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  //        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  //        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
  //        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  //        pool_info.pPoolSizes = pool_sizes;
  //        return vkCreateDescriptorPool(m_device, &pool_info, nullptr,
  //        &m_descriptor_pool);
}
}  // namespace wunder
