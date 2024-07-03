#ifndef WUNDER_MACROS_H
#define WUNDER_MACROS_H

#include "core/wunder_logger.h"

#ifdef WANDER_LINUX
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

#define AssertLogIf(x, ...)                               \
  {                                                       \
    if ((x)) {                                            \
      WUNDER_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
    }                                                     \
  }
#define AssertLogUnless(x, ...)                           \
  {                                                       \
    if (!(x)) {                                           \
      WUNDER_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
    }                                                     \
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

#endif
