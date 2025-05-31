#ifndef WUNDER_LOGGER_H
#define WUNDER_LOGGER_H
#include "core/wunder_memory.h"

_Pragma("GCC diagnostic ignored \"-Wdeprecated-literal-operator\"")
#include <spdlog/spdlog.h>
_Pragma("GCC diagnostic pop")

    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    namespace wunder {
  /////////////////////////////////////////////////////////////////////////////////////////
  class log {
   public:
    static void init();

    static spdlog::logger& get_logger() { return *s_logger; }

    template <typename... Args>
    static void trace_tag(std::string_view tag,
                          std::format_string<Args...> format, Args&&... args);
    template <typename... Args>
    static void debug_tag(std::string_view tag,
                          std::format_string<Args...> format, Args&&... args);
    template <typename... Args>
    static void info_tag(std::string_view tag,
                         std::format_string<Args...> format, Args&&... args);
    template <typename... Args>
    static void warn_tag(std::string_view tag,
                         std::format_string<Args...> format, Args&&... args);
    template <typename... Args>
    static void error_tag(std::string_view tag,
                          std::format_string<Args...> format, Args&&... args);
    template <typename... Args>
    static void fatal_tag(std::string_view tag,
                          std::format_string<Args...> format, Args&&... args);

    static void trace_tag(std::string_view tag, std::string_view message);
    static void debug_tag(std::string_view tag, std::string_view message);
    static void info_tag(std::string_view tag, std::string_view message);
    static void warn_tag(std::string_view tag, std::string_view message);
    static void error_tag(std::string_view tag, std::string_view message);
    static void fatal_tag(std::string_view tag, std::string_view message);

   private:
    log() = default;
    static shared_ptr<spdlog::logger> s_logger;
  };

  template <typename... Args>
  void log::trace_tag(std::string_view tag,
                      const std::format_string<Args...> format,
                      Args&&... args) {
    std::string formatted = std::format(format, std::forward<Args>(args)...);
    trace_tag(tag, formatted);
  }

  template <typename... Args>
  void log::debug_tag(std::string_view tag,
                      const std::format_string<Args...> format,
                      Args&&... args) {
    std::string formatted = std::format(format, std::forward<Args>(args)...);
    debug_tag(tag, formatted);
  }

  template <typename... Args>
  void log::warn_tag(std::string_view tag,
                     const std::format_string<Args...> format, Args&&... args) {
    std::string formatted = std::format(format, std::forward<Args>(args)...);
    warn_tag(tag, formatted);
  }

  template <typename... Args>
  void log::info_tag(std::string_view tag,
                     const std::format_string<Args...> format, Args&&... args) {
    std::string formatted = std::format(format, std::forward<Args>(args)...);
    info_tag(tag, formatted);
  }

  template <typename... Args>
  void log::error_tag(std::string_view tag,
                      const std::format_string<Args...> format,
                      Args&&... args) {
    std::string formatted = std::format(format, std::forward<Args>(args)...);
    error_tag(tag, formatted);
  }

  template <typename... Args>
  void log::fatal_tag(std::string_view tag,
                      const std::format_string<Args...> format,
                      Args&&... args) {
    std::string formatted = std::format(format, std::forward<Args>(args)...);
    fatal_tag(tag, formatted);
  }

}  // namespace wunder

#if defined(WUNDER_ENABLE_LOG)
#define WUNDER_TRACE(...) wunder::log::trace_tag("Unspecified", __VA_ARGS__)
#define WUNDER_DEBUG(...) wunder::log::debug_tag("Unspecified", __VA_ARGS__)
#define WUNDER_INFO(...) wunder::log::info_tag("Unspecified", __VA_ARGS__)
#define WUNDER_WARN(...) wunder::log::warn_tag("Unspecified", __VA_ARGS__)
#define WUNDER_ERROR(...) wunder::log::error_tag("Unspecified", __VA_ARGS__)
#define WUNDER_CRITICAL(...) wunder::log::fatal_tag("Unspecified", __VA_ARGS__)

#define WUNDER_TRACE_TAG(tag, ...) wunder::log::trace_tag(tag, __VA_ARGS__)
#define WUNDER_DEBUG_TAG(tag, ...) wunder::log::debug_tag(tag, __VA_ARGS__)
#define WUNDER_INFO_TAG(tag, ...) wunder::log::info_tag(tag, __VA_ARGS__)
#define WUNDER_WARN_TAG(tag, ...) wunder::log::warn_tag(tag, __VA_ARGS__)
#define WUNDER_ERROR_TAG(tag, ...) wunder::log::error_tag(tag, __VA_ARGS__)
#define WUNDER_CRITICAL_TAG(tag, ...) wunder::log::fatal_tag(tag, __VA_ARGS__)

#else
#define WUNDER_TRACE(...)
#define WUNDER_INFO(...)
#define WUNDER_WARN(...)
#define WUNDER_ERROR(...)
#define WUNDER_CRITICAL(...)
#endif

#endif
