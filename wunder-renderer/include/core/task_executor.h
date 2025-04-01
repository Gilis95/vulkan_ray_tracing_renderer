#ifndef TASK_EXECUTOR_H
#define TASK_EXECUTOR_H
#include <future>
#include <list>
#include <queue>
#include <thread>

#include "core/async_task.h"
#include "core/time_unit.h"
#include "core/wunder_macros.h"

namespace wunder {
class task_executor {
 private:
  struct task_data {
    std::future<void> m_future;
    std::jthread m_thread;

    std::unique_ptr<async_task> m_task;
  };

public:
  explicit task_executor(std::uint32_t pool_size = 1);
 public:
  void update(time_unit dt);

  void schedule(std::unique_ptr<async_task> task);
private:
  void try_start_task();
  void try_finish_task();
private:
  std::queue<unique_ptr< async_task>> m_scheduled_task_queue;
  std::list<task_data>  m_in_progress_tasks;

  uint32_t m_pool_size;
};
}  // namespace wunder

#endif  // TASK_EXECUTOR_H
