#ifndef VULKAN_RENDERER_H
#define VULKAN_RENDERER_H

#include <glad/vulkan.h>

#include <filesystem>
#include <string>
#include <vector>

#include "core/vector_map.h"
#include "core/wunder_memory.h"
#include "gla/renderer_api.h"

namespace wunder {

class vulkan_descriptor_set_manager;
class vulkan_shader;
class vulkan_pipeline;
class vulkan_shader_binding_table;

struct shader_to_compile {
  std::filesystem::path m_shader_path;
  std::function<void(const vulkan_shader&)> m_on_successful_compile;
  bool m_optional = true;
};

class vulkan_renderer : public renderer_api {
 public:
  ~vulkan_renderer() override;

 public:
  [[nodiscard]] const renderer_capabilities& get_capabilities() const override;

  void update(int dt) override;

 protected:
  void init_internal(const renderer_properties& properties) override;

  vector_map<VkShaderStageFlagBits, std::vector<shader_to_compile>>
  get_shaders_for_compilation();

  void create_descriptor_manager(const vulkan_shader& shader);

 private:
  VkSurfaceKHR m_surface = VK_NULL_HANDLE;  // Vulkan window surface
  vector_map<VkShaderStageFlagBits, std::vector<unique_ptr<vulkan_shader>>>
      m_shaders;
  unique_ptr<vulkan_descriptor_set_manager> m_descriptor_set_manager;
  unique_ptr<vulkan_pipeline> m_pipeline;
  unique_ptr<vulkan_shader_binding_table> m_shader_binding_table;
};
}  // namespace wunder
#endif /* VULKAN_RENDERER_H */
