#ifndef WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H
#define WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H

#include "gla/graphic_layer_abstraction_factory.h"

namespace wunder {
    class vulkan_renderer;
    class renderer_api;

    class vulkan_layer_abstraction_factory : public graphic_layer_abstraction_factory {
    protected:
        void init_instance_internal(renderer_properties& properties) override;
    public:
        const renderer_api& get_renderer_api() const override;
    private:
         unique_ptr<vulkan_renderer> m_renderer;
    };

} // wunder

#endif //WUNDER_VULKAN_LAYER_ABSTRACTION_FACTORY_H
