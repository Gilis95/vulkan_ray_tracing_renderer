#include "gla/vulkan/vulkan_physical_device.h"

#include "core/wunder_macros.h"
#include "gla/vulkan/vulkan.h"
#include "gla/vulkan/vulkan_macros.h"
#include "gla/vulkan/vulkan_physical_device_types.h"
#include "gla/vulkan/vulkan_shader_types.h"

namespace wunder::vulkan {
////////////////////////////////////////////////////////////////////////////////////
// Vulkan Physical Device
////////////////////////////////////////////////////////////////////////////////////

physical_device::physical_device() = default;
physical_device::~physical_device() = default;

void physical_device::initialize() {
  AssertReturnUnless(select_gpu() == VkResult::VK_SUCCESS &&
                     "Failed to select physical device");

  load_supported_gpu_extensions();
  init_physical_info(m_device_info, m_physical_device, 1, 3);
  select_queue_family();
  // TODO:: more sophisticated version retrieval!!!!

  m_depth_format = find_depth_format();
  AssertReturnIf(m_depth_format == VkFormat::VK_FORMAT_UNDEFINED);
}

VkResult physical_device::select_gpu() {
  auto& vulkan = layer_abstraction_factory::instance()
                     .get_vulkan_context()
                     .mutable_vulkan();

  uint32_t physical_device_count = 0;
  VK_CHECK_RESULT(vkEnumeratePhysicalDevices(vulkan.get_instance(),
                                             &physical_device_count, nullptr));

  AssertReturnUnless(0 < physical_device_count, VK_ERROR_UNKNOWN);

  // Size the device array appropriately and get the physical
  // device handles.
  std::vector<VkPhysicalDevice> physical_devices(
      physical_device_count);  // list of available gpu`s
  VK_CHECK_RESULT(vkEnumeratePhysicalDevices(
      vulkan.get_instance(), &physical_device_count, &physical_devices[0]));

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

void physical_device::load_supported_gpu_extensions() {
  uint32_t ext_count = 0;
  vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &ext_count,
                                       nullptr);
  if (ext_count > 0) {
    std::vector<VkExtensionProperties> extensions(ext_count);
    if (vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr,
                                             &ext_count, &extensions.front()) ==
        VK_SUCCESS) {
      WUNDER_TRACE_TAG("Renderer",
                       "Selected physical device has {0} extensions",
                       extensions.size());
      for (const auto& ext : extensions) {
        m_supported_extensions.emplace(ext.extensionName);
        WUNDER_TRACE_TAG("Renderer", "  {0}", ext.extensionName);
      }
    }
  }
}

void physical_device::select_queue_family() {
  // Queue families
  // Desired queues need to be requested upon logical device creation
  // Due to differing queue family configurations of Vulkan implementations this
  // can be a bit tricky, especially if the application requests different queue
  // types

  // Get queue family indices for the requested queue family types
  // Note that the indices may overlap depending on the implementation

  static const float defaultQueuePriority(0.0f);

  int requested_queue_types =
      VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
  uint32_t queue_family_count;
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device,
                                           &queue_family_count, nullptr);
  AssertReturnUnless(queue_family_count > 0 && "No queue families available");

  m_queue_family_properties.resize(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(
      m_physical_device, &queue_family_count, m_queue_family_properties.data());

  m_queue_family_indices = get_queue_family_indices(requested_queue_types);

  // Graphics queue
  if (requested_queue_types & VK_QUEUE_GRAPHICS_BIT) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = m_queue_family_indices.Graphics;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &defaultQueuePriority;
    m_queue_create_infos.push_back(queue_create_info);
  }

  // Dedicated compute queue
  if (requested_queue_types & VK_QUEUE_COMPUTE_BIT) {
    if (m_queue_family_indices.Compute != m_queue_family_indices.Graphics) {
      // If compute family index differs, we need an additional queue create
      // info for the compute queue
      VkDeviceQueueCreateInfo queue_create_info{};
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.queueFamilyIndex = m_queue_family_indices.Compute;
      queue_create_info.queueCount = 1;
      queue_create_info.pQueuePriorities = &defaultQueuePriority;
      m_queue_create_infos.push_back(queue_create_info);
    }
  }

  // Dedicated transfer queue
  if (requested_queue_types & VK_QUEUE_TRANSFER_BIT) {
    if ((m_queue_family_indices.Transfer != m_queue_family_indices.Graphics) &&
        (m_queue_family_indices.Transfer != m_queue_family_indices.Compute)) {
      // If compute family index differs, we need an additional queue create
      // info for the compute queue
      VkDeviceQueueCreateInfo queue_create_info{};
      queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_create_info.queueFamilyIndex = m_queue_family_indices.Transfer;
      queue_create_info.queueCount = 1;
      queue_create_info.pQueuePriorities = &defaultQueuePriority;
      m_queue_create_infos.push_back(queue_create_info);
    }
  }
}

VkFormat physical_device::find_depth_format() const {
  // Since all depth formats may be optional, we need to find a suitable depth
  // format to use Start with the highest precision packed format
  std::vector<VkFormat> depth_formats = {VK_FORMAT_D24_UNORM_S8_UINT,
                                         VK_FORMAT_D32_SFLOAT_S8_UINT,
                                         VK_FORMAT_D16_UNORM_S8_UINT};

  for (auto& format : depth_formats) {
    VkFormatProperties format_props{};
    format_props.linearTilingFeatures = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2;

    vkGetPhysicalDeviceFormatProperties(m_physical_device, format,
                                        &format_props);
    // Format must support depth stencil attachment for optimal tiling
    if (format_props.optimalTilingFeatures &
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
      return format;
  }
  return VK_FORMAT_UNDEFINED;
}

bool physical_device::is_extension_supported(
    const std::string& extensionName) const {
  return m_supported_extensions.find(extensionName) !=
         m_supported_extensions.end();
}

physical_device::queue_family_indices physical_device::get_queue_family_indices(
    int flags) const {
  queue_family_indices indices;

  // Dedicated queue for compute
  // Try to find a queue family index that supports compute but not graphics
  if (flags & VK_QUEUE_COMPUTE_BIT) {
    for (uint32_t i = 0; i < m_queue_family_properties.size(); i++) {
      auto& vk_queue_family_properties = m_queue_family_properties[i];
      if ((vk_queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
          ((vk_queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) ==
           0)) {
        indices.Compute = i;
        break;
      }
    }
  }

  // Dedicated queue for transfer
  // Try to find a queue family index that supports transfer but not graphics
  // and compute
  if (flags & VK_QUEUE_TRANSFER_BIT) {
    for (uint32_t i = 0; i < m_queue_family_properties.size(); i++) {
      auto& vk_queue_family_properties = m_queue_family_properties[i];
      if ((vk_queue_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
          ((vk_queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) ==
           0) &&
          ((vk_queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT) ==
           0)) {
        indices.Transfer = i;
        break;
      }
    }
  }

  // For other queue types or if no separate compute queue is present, return
  // the first one to support the requested flags
  for (uint32_t i = 0; i < m_queue_family_properties.size(); i++) {
    if ((flags & VK_QUEUE_TRANSFER_BIT) && indices.Transfer == -1) {
      if (m_queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
        indices.Transfer = i;
    }

    if ((flags & VK_QUEUE_COMPUTE_BIT) && indices.Compute == -1) {
      if (m_queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
        indices.Compute = i;
    }

    if (flags & VK_QUEUE_GRAPHICS_BIT) {
      if (m_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        indices.Graphics = i;
    }
  }

  return indices;
}

uint32_t physical_device::get_memory_type_index(
    uint32_t typeBits, VkMemoryPropertyFlags properties) const {
  // Iterate over all memory types available for the device used in this example
  for (uint32_t i = 0; i < m_device_info.m_memory_properties.memoryTypeCount;
       i++) {
    if ((typeBits & 1) == 1) {
      if ((m_device_info.m_memory_properties.memoryTypes[i].propertyFlags &
           properties) == properties)
        return i;
    }
    typeBits >>= 1;
  }

  AssertReturnUnless("Could not find a suitable memory type!", UINT32_MAX);
  return UINT32_MAX;
}

}  // namespace wunder::vulkan