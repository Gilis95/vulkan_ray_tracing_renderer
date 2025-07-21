#include "core/task_executor.h"

#include "core/wunder_macros.h"
namespace wunder {

task_executor::task_executor(std::uint32_t pool_size) {
  for (uint32_t i = 0; i < pool_size; i++) {
    m_worker_threads.emplace_back(&task_executor::run, this);
  }
}

void task_executor::shutdown() {
  for (auto& worker_thread : m_worker_threads) {
    AssertContinueUnless(worker_thread.request_stop());
  }
  m_scheduled_tasks_cv.notify_all();
}

void task_executor::update(time_unit /*dt*/) { try_finish_task(); }

void task_executor::run(const std::stop_token& token) {
  while (!token.stop_requested()) {
    try_run_task(token);
  }
}

void task_executor::enqueue(async_task* task) {
  AssertReturnUnless(task);

  {
    std::lock_guard lock(m_scheduled_tasks_mutex);
    m_scheduled_tasks_queue.emplace(std::shared_ptr<async_task>(task));
  }

  m_scheduled_tasks_cv.notify_one();
}

void task_executor::try_run_task(const std::stop_token& token) {
  shared_ptr<async_task> task;
  {
    std::unique_lock lock(m_scheduled_tasks_mutex);
    m_scheduled_tasks_cv.wait(lock, [this, &token] {
      return !m_scheduled_tasks_queue.empty() || token.stop_requested();
    });

    ReturnIf(token.stop_requested());

    task = m_scheduled_tasks_queue.front();
    m_scheduled_tasks_queue.pop();
  }

  task->run();

  {
    std::lock_guard ready_task_lock(m_ready_tasks_mutex);
    m_ready_task_queue.emplace(std::move(task));
  }
}

void task_executor::try_finish_task() {
  shared_ptr<async_task> ready_task;
  {
    std::lock_guard ready_task_lock(m_ready_tasks_mutex);
    ReturnIf(m_ready_task_queue.empty());

    ready_task = m_ready_task_queue.front();
    m_ready_task_queue.pop();
  }

  AssertReturnUnless(ready_task);
  ready_task->execute_on_main_thread();
}

}  // namespace wunder
