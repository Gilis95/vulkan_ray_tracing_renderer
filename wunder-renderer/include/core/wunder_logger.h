#ifndef WUNDER_LOGGER_H
#define WUNDER_LOGGER_H

#include "core/wunder_memory.h"
#include <spdlog/spdlog.h>

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
namespace wunder {
    /////////////////////////////////////////////////////////////////////////////////////////
    class log {
    public:
        static void init();

        static spdlog::logger& get_logger() {
            return *s_logger;
        }

    private:
        log() = default;
        static shared_ptr<spdlog::logger> s_logger;
    };
}

#if defined(WUNDER_ENABLE_LOG)
#define WUNDER_TRACE(...)         wunder::log::get_logger().trace(__VA_ARGS__)
#define WUNDER_INFO(...)          wunder::log::get_logger().info(__VA_ARGS__)
#define WUNDER_WARN(...)          wunder::log::get_logger().warn(__VA_ARGS__)
#define WUNDER_ERROR(...)         wunder::log::get_logger().error(__VA_ARGS__)
#define WUNDER_CRITICAL(...)      wunder::log::get_logger().critical(__VA_ARGS__)
#else
#define WUNDER_TRACE(...)
#define WUNDER_INFO(...)
#define WUNDER_WARN(...)
#define WUNDER_ERROR(...)
#define WUNDER_CRITICAL(...)
#endif

#endif
