#ifndef TASK_EXECUTOR_H
#define TASK_EXECUTOR_H
#include <future>
#include <queue>
#include <thread>

#include "core/async_task.h"
#include "core/time_unit.h"
#include "core/wunder_macros.h"

namespace wunder {
class task_executor {
 public:
  explicit task_executor(std::uint32_t pool_size = 1);

  void shutdown();

 public:
  void update(time_unit dt);

  void enqueue(async_task* task);

 private:  // executed on worker threads
  // It seems like stop_toke is simple ptr wrapper, bu however
  void run(const std::stop_token& token);
  void try_run_task(const std::stop_token& token);

 private:  // executed on owner thread
  void try_finish_task();

 private:
  std::queue<shared_ptr<async_task>> m_scheduled_tasks_queue;
  std::mutex m_scheduled_tasks_mutex;
  std::condition_variable m_scheduled_tasks_cv;

  std::queue<shared_ptr<async_task>> m_ready_task_queue;
  std::mutex m_ready_tasks_mutex;

  std::vector<std::jthread> m_worker_threads;
};
}  // namespace wunder

#endif  // TASK_EXECUTOR_H
