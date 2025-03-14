#ifndef VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILDER_H
#define VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILDER_H
#include "gla/vulkan/ray-trace/vulkan_acceleration_structure_builder.h"
#include "gla/vulkan/ray-trace/vulkan_top_level_acceleration_structure_build_info.h"

namespace wunder::vulkan {
class top_level_acceleration_structure;

class top_level_acceleration_structure_builder final
    : protected acceleration_structure_builder<
          top_level_acceleration_structure_build_info> {
 public:
  explicit top_level_acceleration_structure_builder(
      top_level_acceleration_structure& acceleration_structure,
      std::vector<top_level_acceleration_structure_build_info>& build_infos,
      const std::vector<vulkan_mesh_scene_node>& mesh_nodes);

 public:
  void build();

 private:
  void build_info_set_scratch_buffer();
  void create_acceleration_structures();

  void wait_until_instances_buffer_is_available() const;

  void flush_commands();
 protected:
  const std::vector<top_level_acceleration_structure_build_info>&
  get_build_infos() const override {
    return m_build_infos;
  }

 private:
  top_level_acceleration_structure& m_acceleration_structure;
  const std::vector<vulkan_mesh_scene_node>& mesh_nodes;
  std::vector<top_level_acceleration_structure_build_info>& m_build_infos;
};
}  // namespace wunder::vulkan

#endif  // VULKAN_TOP_LEVEL_ACCELERATION_STRUCTURE_BUILDER_H
