#ifndef WUNDER_VULKAN_MESH_H
#define WUNDER_VULKAN_MESH_H

#include <cstdint>

#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure.h"

namespace wunder::vulkan {
struct vulkan_mesh {
  std::uint32_t m_idx;
  std::unique_ptr<storage_buffer> m_vertex_buffer;
  std::unique_ptr<storage_buffer> m_index_buffer;
  bottom_level_acceleration_structure m_blas;
  std::uint32_t m_material_idx;
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_MESH_H
