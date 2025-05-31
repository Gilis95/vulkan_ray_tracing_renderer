#include "gla/vulkan/vulkan_base_renderer.h"

#include "core/wunder_macros.h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_shader.h"

namespace wunder::vulkan {

base_renderer::~base_renderer() = default;

descriptor_set_manager &base_renderer::mutable_descriptor_set_manager() {
  return *m_descriptor_set_manager;
}

const renderer_capabilities &base_renderer::get_capabilities() const {
  return layer_abstraction_factory::instance()
      .get_vulkan_context()
      .get_capabilities();
}

void base_renderer::shutdown() {
  shutdown_internal();

  if (m_descriptor_set_manager) {
    m_descriptor_set_manager.reset();
  }

  for (auto &[_, shaders] : m_shaders) {
    for (auto &shader : shaders) {
      ContinueUnless(shader);
      shader.reset();
    }
  }
  m_shaders.clear();
}

void base_renderer::init(const renderer_properties &properties) {
  initialize_shaders();
  init_internal(properties);
}

void base_renderer::initialize_shaders() {
  for (auto &[shader_type, shaders_compile_data] :
       get_shaders_for_compilation()) {
    auto &shaders_of_type = m_shaders[shader_type];

    for (auto &shader_compile_data : shaders_compile_data) {
      auto maybe_shader =
          shader::create(shader_compile_data.m_shader_path, shader_type);
      if (maybe_shader.has_value()) {
        auto &shader =
            shaders_of_type.emplace_back(std::move(maybe_shader.value()));

        ContinueUnless(shader_compile_data.m_on_successful_compile);
        shader_compile_data.m_on_successful_compile(*shader);
        continue;
      }

      WUNDER_ERROR_TAG("Renderer",
                       "Failed to compile {0} of type {1}. Error:  {2}",
                       shader_compile_data.m_shader_path.string(),
                       static_cast<int>(shader_type),
                       static_cast<int>(maybe_shader.error()));
      ContinueIf(shader_compile_data.m_optional);
      CRASH;
    }
  }
}
}  // namespace wunder::vulkan