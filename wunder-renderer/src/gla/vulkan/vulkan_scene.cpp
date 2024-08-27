#include "gla/vulkan/vulkan_scene.h"

namespace wunder {
void vulkan_scene::start_binding() {}
void vulkan_scene::finish_binding() {}

void vulkan_scene::operator()(const camera_component& component) {}
void vulkan_scene::operator()(const material_component& component) {}
void vulkan_scene::operator()(const mesh_component&) {}
void vulkan_scene::operator()(const transform_component& component) {}
void vulkan_scene::operator()(const light_component& component) {}
}  // namespace wunder