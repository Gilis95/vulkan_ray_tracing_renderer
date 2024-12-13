#ifndef WUNDER_MACROS_H
#define WUNDER_MACROS_H

#include "core/wunder_logger.h"

#ifdef WANDER_LINUX
#include <signal.h>
#define DEBUGBREAK() raise(SIGTRAP);
#elif WANDER_WINDOWS
#define DEBUGBREAK() __debug_breka
#else
#define DEBUGBREAK()
#endif

#ifdef ENABLE_ASSERTS
#define WUNDER_LOG_ERROR_AND_BREAK(...) \
  {                                     \
    WUNDER_ERROR(__VA_ARGS__);          \
    DEBUGBREAK();                       \
  }
#else
#define WUNDER_LOG_ERROR_AND_BREAK(...)
#endif

// DO NOT delete the volatile keyword, otherwise compiler might delete the
// instruction
#define CRASH *((volatile unsigned int*)0) = 0xDEAD

#define AssertLogIf(x, ...)                                             \
  {                                                                     \
    if ((x)) {                                                          \
      WUNDER_ERROR("Assertion Failed: {0} {1} {2}", __FILE__, __LINE__, \
                   __FUNCTION__);                                       \
    }                                                                   \
  }
#define AssertLogUnless(x, ...)                                         \
  {                                                                     \
    if (!(x)) {                                                         \
      WUNDER_ERROR("Assertion Failed: {0} {1} {2}", __FILE__, __LINE__, \
                   __FUNCTION__);                                       \
    }                                                                   \
  }

#define ReturnIf(x, ...)  \
  {                       \
    if (x) {              \
      return __VA_ARGS__; \
    }                     \
  }
#define ReturnUnless(x, ...) \
  {                          \
    if (!(x)) {              \
      return __VA_ARGS__;    \
    }                        \
  }

#define CrashIf(x, ...)                                                       \
  {                                                                           \
    if (x) {                                                                  \
      WUNDER_LOG_ERROR_AND_BREAK(                                             \
          "Forcefully crashing the program: {0} {1} {2}", __FILE__, __LINE__, \
          __FUNCTION__);                                                      \
      CRASH;                                                                  \
    }                                                                         \
  }

#define CrashUnless(x, ...)                                                   \
  {                                                                           \
    if (!(x)) {                                                               \
      WUNDER_LOG_ERROR_AND_BREAK(                                             \
          "Forcefully crashing the program: {0} {1} {2}", __FILE__, __LINE__, \
          __FUNCTION__);                                                      \
      CRASH;                                                                  \
    }                                                                         \
  }

#define AssertReturnIf(x, ...)                                              \
  {                                                                         \
    if (x) {                                                                \
      WUNDER_LOG_ERROR_AND_BREAK("Assertion Failed: {0} {1} {2}", __FILE__, \
                                 __LINE__, __FUNCTION__);                   \
      return __VA_ARGS__;                                                   \
    }                                                                       \
  }
#define AssertReturnUnless(x, ...)                                          \
  {                                                                         \
    if (!(x)) {                                                             \
      WUNDER_LOG_ERROR_AND_BREAK("Assertion Failed: {0} {1} {2}", __FILE__, \
                                 __LINE__, __FUNCTION__);                   \
      return __VA_ARGS__;                                                   \
    }                                                                       \
  }

#define ContinueIf(x) \
  {                   \
    if (x) {          \
      continue;       \
    }                 \
  }
#define ContinueUnless(x) \
  {                       \
    if (!(x)) {           \
      continue;           \
    }                     \
  }

#define AssertContinueIf(x, ...)                                            \
  {                                                                         \
    if (x) {                                                                \
      WUNDER_LOG_ERROR_AND_BREAK("Assertion Failed: {0} {1} {2}", __FILE__, \
                                 __LINE__, __FUNCTION__);                   \
      continue;                                                             \
    }                                                                       \
  }
#define AssertContinueUnless(x, ...)                                        \
  {                                                                         \
    if (!(x)) {                                                             \
      WUNDER_LOG_ERROR_AND_BREAK("Assertion Failed: {0} {1} {2}", __FILE__, \
                                 __LINE__, __FUNCTION__);                   \
      continue;                                                             \
    }                                                                       \
  }

namespace wunder {
template <typename T, typename... Ts>
constexpr bool contains = (std::is_same<T, Ts>{} || ...);

template <typename Subset, typename Set>
constexpr bool is_subset_of = false;

template <typename... Ts, typename... Us>
constexpr bool is_subset_of<std::tuple<Ts...>, std::tuple<Us...>> =
    (contains<Ts, Us...> && ...);
}  // namespace wunder

#endif
