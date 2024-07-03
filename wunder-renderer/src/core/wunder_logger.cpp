#include "core/wunder_logger.h"
#include <iostream>

#include <sstream>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace wunder {
    shared_ptr<spdlog::logger> log::s_logger;

    /////////////////////////////////////////////////////////////////////////////////////////
    void log::init() {
        std::vector<spdlog::sink_ptr> log_sinks;
        log_sinks.emplace_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());
        log_sinks.emplace_back(
                std::make_shared<spdlog::sinks::basic_file_sink_mt>("engine.log", true));

        log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
        log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

        s_logger.reset(new spdlog::logger("wunder-renderer", std::begin(log_sinks), std::end(log_sinks)));
        register_logger(s_logger);
        s_logger->set_level(spdlog::level::trace);
        s_logger->flush_on(spdlog::level::trace);

    }
}
