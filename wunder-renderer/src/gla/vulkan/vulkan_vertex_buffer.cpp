#include "gla/vulkan/vulkan_vertex_buffer.h"

#include <vector>

#include "assets/components/mesh_asset.h"

#include "resources/shaders/compress.glsl"
#include "resources/shaders/host_device.h"

namespace wunder{
vulkan_buffer vulkan_vertex_buffer::create(const mesh_asset& asset)
{
  std::vector<VertexAttributes> vertecies{};
  for (auto& vertex : asset.m_verticies) {
    VertexAttributes device_vertex{};
    device_vertex.position = vertex.m_position;
    device_vertex.normal = compress_unit_vec(vertex.m_normal);
    device_vertex.tangent =
        compress_unit_vec(vertex.m_tangent);  // See .w encoding below
    device_vertex.texcoord = vertex.m_texcoord;
    device_vertex.color = glm::packUnorm4x8(vertex.m_color);

    // Encode to the Less-Significant-Bit the handiness of the tangent
    // Not a significant change on the UV to make a visual difference
    // auto     uintBitsToFloat = [](uint32_t a) -> float { return
    // *(float*)&(a); }; auto     floatBitsToUint = [](float a) -> uint32_t {
    // return *(uint32_t*)&(a); };
    uint32_t value = floatBitsToUint(device_vertex.texcoord.y);
    if (vertex.m_tangent.w > 0) {
      value |= 1;  // set bit, H == +1
    } else {
      value &= ~1;  // clear bit, H == -1
    }
    device_vertex.texcoord.y = uintBitsToFloat(value);

    vertecies.push_back(std::move(device_vertex));
  }

  return vulkan_device_buffer{
      vertecies.data(), vertecies.size() * sizeof(VertexAttributes),
      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
          VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
          VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR};
}

}
