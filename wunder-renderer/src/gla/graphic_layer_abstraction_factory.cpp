#include "gla/graphic_layer_abstraction_factory.h"
#include "gla/vulkan/vulkan_layer_abstraction_factory.h"

namespace wunder {
    void  graphic_layer_abstraction_factory::create_instance(renderer_properties &properties) {
        switch (properties.m_renderer_type) {
            case gla_type::Vulkan:
                s_instance = make_unique<vulkan_layer_abstraction_factory>();
        }

        s_instance->init_instance(properties);
    }

    void graphic_layer_abstraction_factory::init_instance(renderer_properties &properties) {
        init_instance_internal(properties);
    }
}