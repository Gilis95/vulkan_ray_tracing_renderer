#include "gla/vulkan/vulkan_index_buffer.h"

#include "gla/vulkan/vulkan_device_buffer.h"
#include "include/assets/mesh_asset.h"

namespace wunder::vulkan {
std::unique_ptr< storage_buffer> index_buffer::create(const mesh_asset& asset) {
  return std::make_unique< storage_device_buffer>(
      descriptor_build_data{.m_enabled = false},
      asset.m_indecies.data(), asset.m_indecies.size() * sizeof(std::uint32_t),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR);
}
}  // namespace wunder