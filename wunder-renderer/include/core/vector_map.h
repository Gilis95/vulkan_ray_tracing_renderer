//
// Created by christian on 8/7/24.
//

#ifndef WUNDER_VECTOR_MAP_H
#define WUNDER_VECTOR_MAP_H

#include <algorithm>
#include <concepts>
#include <utility>
#include <vector>

#include "wunder_macros.h"

namespace wunder {
template <std::equality_comparable key_t, typename value_t>
class vector_map : public std::vector<std::pair<key_t, value_t>> {
 private:
  using base_type = std::vector<std::pair<key_t, value_t>>;

 public:
  [[nodiscard]] base_type::iterator find(const key_t& key) {
    return std::find_if(base_type::begin(), base_type::end(),
                     [&key](const std::pair<key_t, value_t>& element) {
                       return element.first == key;
                     });
  }

  [[nodiscard]] value_t& operator[](const key_t& key) {
    auto found_it = find(key);
    ReturnIf(found_it != base_type::end(), found_it->second);

    return base_type::emplace_back(std::make_pair(key, value_t())).second;
  }
};
}  // namespace wunder
#endif  // WUNDER_VECTOR_MAP_H
