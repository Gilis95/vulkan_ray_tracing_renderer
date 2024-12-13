//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_ENTITY_H
#define WUNDER_ENTITY_H

#include <optional>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

#include "core/wunder_macros.h"

namespace wunder {

template <typename... types>
class entity {
 public:
  template <typename visitor_type>
  void iterate_components(visitor_type& visitor);

  template <typename component_type>
  void add_component(component_type&& component);

  template <typename component_type>
  [[nodiscard]] bool has_component() const;

  template <typename... component_types>
  [[nodiscard]] bool has_components() const;

  template <typename component_type>
  optional_ref<component_type> mutable_component();

  template <typename component_type>
  optional_const_ref<component_type> get_component() const;

 private:
  std::vector<std::variant<types...>> m_components;
};

template <typename... types>
template <typename visitor_type>
void entity<types...>::iterate_components(visitor_type& visitor) {
  for (const auto& component : m_components) {
    std::visit(visitor, component);
  }
}

template <typename... types>
template <typename component_type>
void entity<types...>::add_component(component_type&& component) {
  m_components.emplace_back(component);
}

template <typename... types>
template <typename component_type>
bool entity<types...>::has_component() const {
  for(auto& component : m_components){
     ReturnIf(std::holds_alternative<component_type>(component), true)
  }

  return false;
}

template <typename... types>
template <typename... component_types>
bool entity<types...>::has_components() const {
  return ((has_component<component_types>()) && ...);
}

template <typename... types>
template <typename component_type>
optional_ref<component_type> entity<types...>::mutable_component() {
  for (auto& component : m_components) {
    auto result = std::get_if<component_type>(&component);
    ReturnIf(result, *result);
  }

  return std::nullopt;
}

template <typename... types>
template <typename component_type>
optional_const_ref<component_type> entity<types...>::get_component() const {
  for (auto& component : m_components) {
    auto result = std::get_if<component_type>(&component);
    ReturnIf(result, *result);
  }

  return std::nullopt;
}
}  // namespace wunder
#endif  // WUNDER_ENTITY_H
