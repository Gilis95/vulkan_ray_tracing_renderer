#ifndef WUNDER_VULKAN_MESH_H
#define WUNDER_VULKAN_MESH_H

#include <cstdint>

#include "gla/vulkan/ray-trace/vulkan_bottom_level_acceleration_structure.h"

namespace wunder::vulkan {
struct vulkan_mesh {
  std::uint32_t m_idx;
  std::unique_ptr<storage_buffer> m_vertex_buffer;
  std::uint32_t m_vertices_count;
  std::unique_ptr<storage_buffer> m_index_buffer;
  std::uint32_t m_indices_count;
  bottom_level_acceleration_structure m_blas;
  std::uint32_t m_material_idx;
  bool m_is_opaque;
  bool m_is_double_sided;
};
}  // namespace wunder::vulkan

#endif  // WUNDER_VULKAN_MESH_H
