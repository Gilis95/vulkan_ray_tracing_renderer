#ifndef WUNDER_INPUT_H
#define WUNDER_INPUT_H

#include <cstdint>
#include <variant>

namespace wunder {

namespace keyboard {
enum class key_code : uint16_t {
  // From glfw3.h
  space = 32,
  apostrophe = 39, /* ' */
  comma = 44,      /* , */
  minus = 45,      /* - */
  period = 46,     /* . */
  slash = 47,      /* / */

  d_0 = 48, /* 0 */
  d_1 = 49, /* 1 */
  d_2 = 50, /* 2 */
  d_3 = 51, /* 3 */
  d_4 = 52, /* 4 */
  d_5 = 53, /* 5 */
  d_6 = 54, /* 6 */
  d_7 = 55, /* 7 */
  d_8 = 56, /* 8 */
  d_9 = 57, /* 9 */

  semicolon = 59, /* , */
  equal = 61,     /* = */

  a = 65,
  b = 66,
  c = 67,
  d = 68,
  e = 69,
  f = 70,
  g = 71,
  h = 72,
  i = 73,
  j = 74,
  k = 75,
  l = 76,
  m = 77,
  n = 78,
  o = 79,
  p = 80,
  q = 81,
  r = 82,
  s = 83,
  t = 84,
  u = 85,
  v = 86,
  w = 87,
  x = 88,
  y = 89,
  z = 90,

  LeftBracket = 91,  /* [ */
  Backslash = 92,    /* \ */
  RightBracket = 93, /* ] */
  GraveAccent = 96,  /* ` */

  World1 = 161, /* non-US #1 */
  World2 = 162, /* non-US #2 */

  escape = 256,
  enter = 257,
  tab = 258,
  backspace = 259,
  insert = 260,
  _delete = 261,
  right = 262,
  left = 263,
  down = 264,
  up = 265,
  page_up = 266,
  page_down = 267,
  home = 268,
  end = 269,
  caps_lock = 280,
  scroll_lock = 281,
  num_lock = 282,
  print_screen = 283,
  pause = 284,
  f_1 = 290,
  f_2 = 291,
  f_3 = 292,
  f_4 = 293,
  f_5 = 294,
  f_6 = 295,
  f_7 = 296,
  f_8 = 297,
  f_9 = 298,
  f_10 = 299,
  f_11 = 300,
  f_12 = 301,
  f_13 = 302,
  f_14 = 303,
  f_15 = 304,
  f_16 = 305,
  f_17 = 306,
  f_18 = 307,
  f_19 = 308,
  f_20 = 309,
  f_21 = 310,
  f_22 = 311,
  f_23 = 312,
  f_24 = 313,
  f_25 = 314,

  kp_0 = 320,
  kp_1 = 321,
  kp_2 = 322,
  kp_3 = 323,
  kp_4 = 324,
  kp_5 = 325,
  kp_6 = 326,
  kp_7 = 327,
  kp_8 = 328,
  kp_9 = 329,
  kp_decimal = 330,
  kp_divide = 331,
  kp_multiply = 332,
  kp_subtract = 333,
  kp_add = 334,
  kp_enter = 335,
  kp_equal = 336,

  left_shift = 340,
  left_control = 341,
  left_alt = 342,
  left_super = 343,
  right_shift = 344,
  right_control = 345,
  right_alt = 346,
  right_super = 347,
  menu = 348
};
}

namespace mouse {
/////////////////////////////////////////////////////////////////////////////////////////
enum class key_code : uint16_t {
  // From glfw3.h
  button_0 = 0,
  button_1 = 1,
  button_2 = 2,
  button_3 = 3,
  button_4 = 4,
  button_5 = 5,
  button_6 = 6,
  button_7 = 7,

  button_last = button_7,
  button_left = button_0,
  button_right = button_1,
  button_middle = button_2
};
}  // namespace mouse

using input_key_code =
    std::variant<wunder::keyboard::key_code, wunder::mouse::key_code>;

}  // namespace wunder

#endif  // WUNDER_INPUT_H
