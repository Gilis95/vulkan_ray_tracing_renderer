#include "application.h"

#include "core/wunder_macros.h"
#include "window/window.h"
#include "window/window_factory.h"

#include <tracy/Tracy.hpp>

namespace wunder {
    /////////////////////////////////////////////////////////////////////////////////////////
    application::application(application_properties properties)
           : m_is_running(false)
           , m_properties(properties) {
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    application::~application() = default;


    /////////////////////////////////////////////////////////////////////////////////////////
    void application::init() {
        std::string application_revision("Revision: 1234");
        std::string application_name("application Name: wunder");

        TracyAppInfo(m_properties.m_debug_name.c_str(), m_properties.m_debug_name.size());
        TracyAppInfo(m_properties.m_debug_version.c_str(), m_properties.m_debug_version.size());

        AssertReturnUnless(window_factory::get_instance().create_window(m_properties.m_window_properties.m_type));
        auto& m_window = window_factory::get_instance().get_window();
        m_window.init(m_properties.m_window_properties);

        init_internal();
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    void application::close() {
        m_is_running = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    void application::run() {
        m_is_running = true;

        while (m_is_running) {
            m_window->update(0);
            FrameMark;
        }
    }
}
