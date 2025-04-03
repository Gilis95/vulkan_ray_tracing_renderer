#ifndef WUNDER_TIME_UNIT_H
#define WUNDER_TIME_UNIT_H

#include  <cstdint>

namespace wunder {
class time_unit {
 public:
  using value_type = std::uint64_t;
 public:
  time_unit();
  explicit time_unit(std::uint64_t miliseconds);
  time_unit(const time_unit& other);
  time_unit(time_unit&& other) noexcept;
  public:
  [[nodiscard]] static time_unit from_current_time_in_miliseconds();
  [[nodiscard]] static time_unit from_current_time_in_seconds();
  [[nodiscard]] static time_unit from_current_time_in_minutes();
  [[nodiscard]] static time_unit from_current_time_in_hours();
 public:
  [[nodiscard]] value_type as_miliseconds() const;
  [[nodiscard]] value_type as_seconds() const;
  [[nodiscard]] value_type as_minutes() const;
  [[nodiscard]] value_type as_hours() const;
  [[nodiscard]] value_type as_days() const;

 public:

  time_unit& operator=(const time_unit& other);
  time_unit& operator=(time_unit&& other) noexcept;
  time_unit& operator-=(time_unit other);
  time_unit& operator+=(time_unit other);

  time_unit operator-(time_unit right) const;
  time_unit operator+(time_unit right) const;
 public:
   value_type m_miliseconds;
};
}  // namespace wunder
#endif  // WUNDER_TIME_UNIT_H
