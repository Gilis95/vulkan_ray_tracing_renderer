#ifndef GLFW_WINDOW_H
#define GLFW_WINDOW_H

#include <utility>
#include "window/window.h"

class GLFWwindow;

namespace wunder {

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class glfw_window : public window {
    public:
        glfw_window();
        ~glfw_window() override;

        void init(const window_properties& properties) override;

        void update(int dt) override;

        void shutdown() override;
    private:
        GLFWwindow*        m_window;
    };
}

#endif
