#ifndef WINDOW_H
#define WINDOW_H

#include <utility>
#include <cstdint>
#include <string>

namespace wunder {
    enum class window_type : std::int32_t {
         glfw
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    struct window_properties {
      public:
        std::string      m_title;
        std::uint32_t    m_width;
        std::uint32_t    m_height;
        window_type      m_type;

        window_properties(std::string title = "wunder application", std::uint32_t width = 1280, std::uint32_t height = 720, window_type type = window_type::glfw)
                : m_title(std::move(title))
                , m_width(width)
                , m_height(height)
                , m_type(type)
        {}
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    class window {
    public:
        window() = default;
        virtual ~window() = default;

        virtual void init(const window_properties&) = 0;

        virtual void update(int dt) = 0;

        virtual void shutdown() = 0;
    };
}

#endif
