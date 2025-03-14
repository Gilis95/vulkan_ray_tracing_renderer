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

template <typename T>
using ref = std::reference_wrapper<T>;

template <typename T>
using const_ref = std::reference_wrapper<const T>;

template <typename T>
using optional_const_ref = std::optional<std::reference_wrapper<const T>>;

template <typename T>
using optional_ref = std::optional<std::reference_wrapper<T>>;

template <typename integral>
constexpr integral align_up(integral x, size_t a) noexcept {
  return integral((x + (integral(a) - 1)) & ~integral(a - 1));
}

}  // namespace wunder

#endif
