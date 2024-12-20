#ifndef WUNDER_KEYBOARD_EVENTS_H
#define WUNDER_KEYBOARD_EVENTS_H

#include "core/input.h"
#include "glm/vec2.hpp"

namespace wunder::event {
namespace keyboard {

struct base {
  explicit base(wunder::keyboard::key_code key_code) : m_key(key_code) {}
  wunder::keyboard::key_code m_key;
};

struct pressed : public base {
 public:
  explicit pressed(wunder::keyboard::key_code key) : base(key) {}
};

struct released : public base {
 public:
  explicit released(wunder::keyboard::key_code key) : base(key) {}
};

struct symbol_pressed : public base {
 public:
  explicit symbol_pressed(wunder::keyboard::key_code key) : base(key) {}
};
}  // namespace keyboard

namespace mouse {

struct scroll {
  glm::vec2 m_offset;
};

struct move {
  glm::vec2 m_position;
};

struct button_base {
  explicit button_base(wunder::mouse::key_code key_code) : m_key(key_code) {}

  wunder::mouse::key_code m_key;
};

struct pressed : public button_base {
  explicit pressed(wunder::mouse::key_code key_code) : button_base(key_code) {}
};

struct released : public button_base {
  explicit released(wunder::mouse::key_code key_code) : button_base(key_code) {}
};
}  // namespace mouse

}  // namespace wunder::event

#endif  // WUNDER_KEYBOARD_EVENTS_H
