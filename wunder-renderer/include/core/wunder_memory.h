#ifndef WUNDER_MEMORY_H
#define WUNDER_MEMORY_H

#ifdef WUNDER_MEMORY_DEBUG
#include <iostream>
#endif
#include <memory>

namespace wunder {
template <typename T>
using shared_ptr = std::shared_ptr<T>;

template <typename T>
using unique_ptr = std::unique_ptr<T>;

template <typename T, typename... Args>
constexpr unique_ptr<T> make_unique(Args &&...args) {
#ifdef WUNDER_MEMORY_DEBUG
  std::cout << "Unique pointer creation: " << __builtin_FUNCTION() << std::endl;
#endif
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using shared_ptr = std::shared_ptr<T>;

template <typename T, typename... Args>
constexpr shared_ptr<T> make_shared(Args &&...args) {
#ifdef WUNDER_MEMORY_DEBUG
  std::cout << "Shared pointer creation: " << __builtin_FUNCTION() << std::endl;
#endif
  return std::make_shared<T>(std::forward<Args>(args)...);
}
}  // namespace wunder

#endif
