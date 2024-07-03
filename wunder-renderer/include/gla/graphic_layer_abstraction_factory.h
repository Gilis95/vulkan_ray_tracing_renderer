#ifndef GLA_FACTORY
#define GLA_FACTORY

#include "core/wunder_memory.h"


namespace wunder{

    enum class gla_type
    {
        Inavlid = -1,
        Vulkan,
        Count
    };

    enum class gpu_to_use
    {
        Invalid = -1,
        Integrated,
        Dedicated,
        Count
    };

    struct renderer_properties
    {
    public:
        gla_type    m_renderer_type;
        gpu_to_use  m_gpu_to_use;
    };

    class renderer_api;

    class graphic_layer_abstraction_factory{
    private:
        graphic_layer_abstraction_factory() = default;
    public:
        static void create_instance(renderer_properties& properties);
        static graphic_layer_abstraction_factory get_instance();

    public:
        virtual const renderer_api& get_renderer_api() const = 0;
    protected:
        virtual void init_instance_internal(renderer_properties& properties) = 0;
    private:
        void init_instance(renderer_properties& properties);
    private:
        static unique_ptr<graphic_layer_abstraction_factory> s_instance;
    };
}

#endif