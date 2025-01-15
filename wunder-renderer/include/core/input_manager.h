//
// Created by christian on 12/18/24.
//

#ifndef WUNDER_INPUT_MANAGER_H
#define WUNDER_INPUT_MANAGER_H

#include <glm/vec2.hpp>
#include <unordered_map>

#include "core/input.h"
#include "event/event_handler.h"
#include "event/input_events.h"

#include "core/wunder_macros.h"

namespace wunder {

class time_unit;

enum class key_state : std::uint8_t {
  none = 1 << 0,
  pressed = 1 << 1,
  held = 1 << 2,
  released = 1 << 3
};

DEFINE_ENUM_FLAG_OPERATIONS(key_state, std::uint8_t)

struct key_data {
  wunder::input_key_code m_key;
  key_state m_state = key_state::none;
  key_state m_old_state = key_state::none;
};

struct mouse_data {
  glm::vec2 m_position;
  glm::vec2 m_old_position;
};

class input_manager
    : private event_handler<wunder::event::keyboard::pressed>,
      private event_handler<wunder::event::keyboard::symbol_pressed>,
      private event_handler<wunder::event::keyboard::released>,
      private event_handler<wunder::event::mouse::pressed>,
      private event_handler<wunder::event::mouse::released>,
      private event_handler<wunder::event::mouse::move> {
 public:
  input_manager();
  ~input_manager() override;

 public:
  void update(time_unit dt);

  bool is_key_in_state(wunder::input_key_code key_code, key_state state);

  float get_mouse_x() const { return m_mouse_data.m_position.x; }
  float get_mouse_y() const { return m_mouse_data.m_position.y; }

  const glm::vec2& get_old_mouse_position() const {
    return m_mouse_data.m_old_position;
  }
  const glm::vec2& get_mouse_position() const {
    return m_mouse_data.m_position;
  }

 private:
  void on_event(const wunder::event::keyboard::pressed& event) override;
  void on_event(const wunder::event::keyboard::symbol_pressed& event) override;
  void on_event(const wunder::event::keyboard::released& event) override;
  void on_event(const wunder::event::mouse::pressed& event) override;
  void on_event(const wunder::event::mouse::released& event) override;
  void on_event(const wunder::event::mouse::move& event) override;

 private:
  key_data& get_or_create_key_data(wunder::input_key_code key_code);
  void change_key_state(wunder::input_key_code key_code, key_state new_state);

  // Internal use only...
  void transition_pressed_keys();
  void clear_released_keys();

 private:
  std::unordered_map<wunder::input_key_code, key_data> m_key_data;
  mouse_data m_mouse_data;
};
}  // namespace wunder
#endif  // WUNDER_INPUT_MANAGER_H
