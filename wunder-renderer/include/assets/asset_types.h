#ifndef WUNDER_ASSET_TYPES_H
#define WUNDER_ASSET_TYPES_H

#include <cstdint>
#include <functional>
#include <unordered_set>

#include "core/vector_map.h"
#include "core/wunder_memory.h"

namespace wunder {
enum class asset_serialization_result_codes {
  ok = 0,
  warning = 1000,
  error = 10000,
  not_supported_format_error = 10001,
  serialization_error = 10002
};

class asset_handle {
 public:
  using type = std::uint32_t;

 public:
  static type s_invalid;

  static asset_handle invalid();
 public:
  asset_handle() ;
  explicit asset_handle(type value) ;

  asset_handle(const asset_handle& other);
  asset_handle& operator=(const asset_handle& other);

  ~asset_handle();

 public:
  [[nodiscard]] bool is_valid() const;
  [[nodiscard]] operator bool() const;

  [[nodiscard]] type value() const;
  [[nodiscard]] explicit operator type() const;

  [[nodiscard]] bool operator==(type other_value) const;

  [[nodiscard]] bool operator==(asset_handle other_value) const;

 private:
  type m_value;
};

template<typename asset_type>
using assets = vector_map<asset_handle, const_ref<asset_type>>;

using asset_ids = std::unordered_set<asset_handle>;

}  // namespace wunder

namespace std {
template <>
struct hash<wunder::asset_handle> {
  size_t operator()(wunder::asset_handle handle) const noexcept {
    return hash<wunder::asset_handle::type>{}(handle.value());
  }
};
}  // namespace std
#endif  // WUNDER_ASSET_TYPES_H
