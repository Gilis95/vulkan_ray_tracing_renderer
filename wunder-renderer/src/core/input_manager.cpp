#include "core/input_manager.h"

#include "core/time_unit.h"
#include "core/wunder_macros.h"
#include "event/event_handler.hpp"

namespace wunder {
input_manager::input_manager()
    : event_handler<wunder::event::keyboard::pressed>(),
      event_handler<wunder::event::keyboard::symbol_pressed>(),
      event_handler<wunder::event::keyboard::released>(),
      event_handler<wunder::event::mouse::pressed>(),
      event_handler<wunder::event::mouse::released>(),
      event_handler<wunder::event::mouse::move>() {}

input_manager::~input_manager() = default;

void input_manager::update(time_unit /*dt*/) {
  transition_pressed_keys();
  clear_released_keys();
}

bool input_manager::is_key_in_state(wunder::input_key_code key_code,
                                    key_state state) {
  auto key_data_it = m_key_data.find(key_code);
  ReturnIf(key_data_it == m_key_data.end(), false);

  return (key_data_it->second.m_state & state) > key_state::none;
}

void input_manager::on_event(
    const wunder::event::keyboard::pressed& event) /*override*/ {
  change_key_state(event.m_key, key_state::pressed);
}

void input_manager::on_event(
    const wunder::event::keyboard::symbol_pressed& event) /*override*/ {
  change_key_state(event.m_key, key_state::pressed);
}

void input_manager::on_event(
    const wunder::event::keyboard::released& event) /*override*/ {
  change_key_state(event.m_key, key_state::released);
}

void input_manager::on_event(
    const wunder::event::mouse::pressed& event) /*override*/ {
  change_key_state(event.m_key, key_state::pressed);
}

void input_manager::on_event(
    const wunder::event::mouse::released& event) /*override*/ {
  change_key_state(event.m_key, key_state::released);
}

void input_manager::on_event(
    const wunder::event::mouse::move& event) /*override*/ {
  m_mouse_data.m_old_position = m_mouse_data.m_position;
  m_mouse_data.m_position = event.m_position;
}

key_data& input_manager::get_or_create_key_data(
    wunder::input_key_code key_code) {
  auto key_data_it = m_key_data.find(key_code);
  ReturnUnless(key_data_it == m_key_data.end(), key_data_it->second);

  key_data& key_data = m_key_data[key_code];
  key_data.m_key = key_code;
  key_data.m_state = key_state::none;
  key_data.m_old_state = key_state::none;

  return key_data;
}

void input_manager::change_key_state(wunder::input_key_code key_code,
                                     key_state new_state) {
  auto& key_data = get_or_create_key_data(key_code);

  key_data.m_old_state = key_data.m_state;
  key_data.m_state = new_state;
}

void input_manager::transition_pressed_keys() {
  for (const auto& [key, key_data] : m_key_data) {
    if (key_data.m_state == key_state::pressed)
      change_key_state(key, key_state::held);
  }
}
void input_manager::clear_released_keys() {
  for (const auto& [key, key_data] : m_key_data) {
    if (key_data.m_state == key_state::released)
      change_key_state(key, key_state::none);
  }
}

}  // namespace wunder