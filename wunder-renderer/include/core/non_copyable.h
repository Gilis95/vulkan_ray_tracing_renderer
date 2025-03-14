//
// Created by christian on 9/13/24.
//

#ifndef WUNDER_NON_COPYABLE_H
#define WUNDER_NON_COPYABLE_H
namespace wunder {
class non_copyable {
 public:
  non_copyable() = default;
  virtual ~non_copyable() = default;

 public:
  non_copyable(const non_copyable&) = delete;
  non_copyable& operator=(const non_copyable&) = delete;

 public:
  non_copyable(non_copyable&&) = default;
  non_copyable& operator=(non_copyable&&) = default;
};
}  // namespace wunder
#endif  // WUNDER_NON_COPYABLE_H
