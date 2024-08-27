//
// Created by christian on 8/23/24.
//

#ifndef WUNDER_ENTITY_H
#define WUNDER_ENTITY_H

#include <optional>
#include <variant>
#include <vector>

namespace wunder {

template <typename... types>
class entity {
 public:
  template <typename visitor_type>
  void iterate_components(visitor_type& visitor);

  template <typename component_type>
  void add_component(component_type&& component);

  template <typename component_type>
  void has_component();

  template <typename component_type>
  std::optional<std::reference_wrapper<component_type>> get_component();

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
void entity<types...>::add_component(component_type&& component)
{
  m_components.emplace_back(component);
}

template <typename... types>
template <typename component_type>
void entity<types...>::has_component() {
  std::holds_alternative<component_type>(m_components);
}

template <typename... types>
template <typename component_type>
std::optional<std::reference_wrapper<component_type>>
entity<types...>::get_component() {
  auto result = std::get_if<component_type>(m_components);
  return result ? std::nullopt : *result;
}

}  // namespace wunder
#endif  // WUNDER_ENTITY_H
