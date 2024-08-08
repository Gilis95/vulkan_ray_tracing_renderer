#include "core/wunder_logger.h"

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <iostream>
#include <memory>

namespace wunder {
shared_ptr<spdlog::logger> log::s_logger;

/////////////////////////////////////////////////////////////////////////////////////////
void log::init() {
  std::vector<spdlog::sink_ptr> log_sinks;
  log_sinks.emplace_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());
  log_sinks.emplace_back(
      make_shared<spdlog::sinks::basic_file_sink_mt>("engine.log", true));

  log_sinks[0]->set_pattern("%^[%T][%l]%v%$");
  log_sinks[1]->set_pattern("[%T][%l]%v");

  s_logger = std::make_shared<spdlog::logger>(
      "wunder-renderer", std::begin(log_sinks), std::end(log_sinks));
  register_logger(s_logger);
  s_logger->set_level(spdlog::level::trace);
  s_logger->flush_on(spdlog::level::trace);
}


void log::trace_tag(std::string_view tag, std::string_view message) {
  s_logger->trace("[{0}] {1}", tag, message);
}

void log::debug_tag(std::string_view tag, std::string_view message) {
  s_logger->debug("[{0}] {1}", tag, message);
}

void log::info_tag(std::string_view tag, std::string_view message) {
  s_logger->info("[{0}] {1}", tag, message);
}

void log::warn_tag(std::string_view tag, std::string_view message) {
  s_logger->warn("[{0}] {1}", tag, message);
}

void log::error_tag(std::string_view tag, std::string_view message) {
  s_logger->error("[{0}] {1}", tag, message);
}

void log::fatal_tag(std::string_view tag, std::string_view message) {
  s_logger->critical("[{0}] {1}", tag, message);
}

}  // namespace wunder
