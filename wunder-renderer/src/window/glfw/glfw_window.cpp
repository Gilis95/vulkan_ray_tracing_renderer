#include "window/glfw/glfw_window.h"

#include "core/wunder_logger.h"
#include "core/wunder_macros.h"

#include <GLFW/glfw3.h>

namespace wunder {

    /////////////////////////////////////////////////////////////////////////////////////////
    static void glfw_error_callback(int error, const char *description) {
        WUNDER_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    glfw_window::glfw_window() = default;
 
    /////////////////////////////////////////////////////////////////////////////////////////
    glfw_window::~glfw_window() = default;

    /////////////////////////////////////////////////////////////////////////////////////////
    void glfw_window::init(const window_properties& properties)
    {
        int status = glfwInit();

        AssertReturnUnless(status);
        glfwSetErrorCallback(glfw_error_callback);

        m_window = glfwCreateWindow(properties.m_width, properties.m_height, properties.m_title.c_str(), NULL, NULL);

        // create context for current window
        glfwMakeContextCurrent(m_window);

        //VSync
        glfwSwapInterval(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    void glfw_window::update(int dt)
    {
        //poll for process events
        glfwPollEvents();

        // swap front and back buffer
        glfwSwapBuffers(m_window);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    void glfw_window::shutdown()
    {
        glfwTerminate();
    }
}
