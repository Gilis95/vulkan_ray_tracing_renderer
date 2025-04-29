#ifndef WUNDER_VULKAN_BASE_RENDERER_H
#define WUNDER_VULKAN_BASE_RENDERER_H

#include <glad/vulkan.h>

#include <filesystem>
#include <functional>
#include <vector>

#include "core/vector_map.h"

namespace wunder::vulkan {
class shader;
class descriptor_set_manager;
}

namespace wunder::vulkan {
struct shader_to_compile {
  std::filesystem::path m_shader_path;
  std::function<void(const shader&)> m_on_successful_compile;
  bool m_optional = true;
};

class base_renderer {
 public:
  virtual ~base_renderer() = default;
  descriptor_set_manager& mutable_descriptor_set_manager();
 protected:
  void initialize_shaders();

 protected:
  virtual vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
  get_shaders_for_compilation() = 0;
 protected:
  vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>> m_shaders;
  unique_ptr<descriptor_set_manager> m_descriptor_set_manager;

};
};      // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_BASE_RENDERER_H
