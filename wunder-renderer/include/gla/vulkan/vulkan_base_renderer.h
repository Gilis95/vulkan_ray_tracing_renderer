#ifndef WUNDER_VULKAN_BASE_RENDERER_H
#define WUNDER_VULKAN_BASE_RENDERER_H

#include <glad/vulkan.h>

#include <filesystem>
#include <functional>
#include <vector>

#include "core/vector_map.h"
#include "scene/scene_types.h"

namespace wunder {
struct renderer_capabilities;
class time_unit;
struct renderer_properties;
}
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
  virtual ~base_renderer();

  descriptor_set_manager& mutable_descriptor_set_manager();
  [[nodiscard]] const renderer_capabilities& get_capabilities() const;

protected:


public:
  void shutdown();
  void init(scene_id scene_id);

protected:
  virtual void shutdown_internal() = 0;
  virtual void init_internal(scene_id scene_id) = 0;

  virtual vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
  get_shaders_for_compilation() = 0;
private:
  void initialize_shaders();

protected:
  vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<shader>>> m_shaders;
  unique_ptr<descriptor_set_manager> m_descriptor_set_manager;
};
};      // namespace wunder::vulkan
#endif  // WUNDER_VULKAN_BASE_RENDERER_H
