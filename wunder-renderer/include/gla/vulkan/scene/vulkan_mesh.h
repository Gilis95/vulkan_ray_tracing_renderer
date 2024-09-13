#ifndef WUNDER_VULKAN_MESH_H
#define WUNDER_VULKAN_MESH_H

#include <cstdint>

#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure.h"
#include "gla/vulkan/vulkan_buffer.h"

namespace wunder {
struct vulkan_mesh {
  std::uint32_t idx;
  vulkan_buffer m_vertex_buffer;
  vulkan_buffer m_index_buffer;
  vulkan_bottom_level_acceleration_structure m_blas;
};
}  // namespace wunder
#endif  // WUNDER_VULKAN_MESH_H
