#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

#include <functional>
#include <initializer_list>
#include <memory>

#include "application_properties.h"
#include "core/vector_map.h"
#include "gla/renderer_capabilities .h"
#include "gla/renderer_properties.h"
#include "gla/vulkan/descriptors/vulkan_descriptor_set_manager.h"
#include "gla/vulkan/ray-trace/vulkan_rtx_renderer.h"
#include "gla/vulkan/vulkan_context.h"
#include "gla/vulkan/vulkan_physical_device.h"
#include "gla/vulkan/vulkan_renderer_context.h"
#include "gla/vulkan/vulkan_shader.h"


namespace wunder::vulkan {
layer_abstraction_factory::layer_abstraction_factory() = default;

layer_abstraction_factory::~layer_abstraction_factory() = default;

layer_abstraction_factory &layer_abstraction_factory::instance() {
  static layer_abstraction_factory s_instance;
  return s_instance;
}

void layer_abstraction_factory::init(
    const application_properties &properties) {
  create_vulkan_context(properties.m_renderer_properties);
  create_renderer(properties);
}

void layer_abstraction_factory::create_renderer(
    const application_properties &properties) {

  m_renderer_context = std::make_unique<renderer_context>(properties);
  m_renderer_context->init();
}

void layer_abstraction_factory::create_vulkan_context(
    const renderer_properties &properties) {
  m_context = std::make_unique<context>();
  m_context->init(properties);
}

renderer_context &layer_abstraction_factory::get_render_context() { return *m_renderer_context; }

context &layer_abstraction_factory::get_vulkan_context() { return *m_context; }

void layer_abstraction_factory::begin_shutdown() {
  if (m_renderer_context) {
    m_renderer_context->shutdown();
    m_renderer_context.reset();
  }
}

void layer_abstraction_factory::end_shutdown() {
  if (m_context.get()) {
    m_context->shutdown();
    m_context.reset();
  }
}
}  // namespace wunder::vulkan