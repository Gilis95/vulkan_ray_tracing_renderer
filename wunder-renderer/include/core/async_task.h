
#ifndef TASK_H
#define TASK_H
namespace wunder {
class async_task {
 public:
  virtual ~async_task() = default;

 public:
  void virtual run(){};

  void virtual execute_on_main_thread(){};
};
}  // namespace wunder
#endif  // TASK_H
