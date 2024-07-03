#include "gla/vulkan/vulkan_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_renderer.h"

namespace wunder {
    void vulkan_layer_abstraction_factory::init_instance_internal(renderer_properties& properties)
    {
        m_renderer = make_unique<vulkan_renderer>();
        m_renderer->init(properties);
    }

    const renderer_api& vulkan_layer_abstraction_factory::get_renderer_api() const
    {
        return *m_renderer;
    }

} // wunder