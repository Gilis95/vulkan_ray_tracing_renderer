#include "core/task_executor.h"

#include "core/wunder_macros.h"
namespace wunder {

task_executor::task_executor(std::uint32_t pool_size)
    : m_pool_size(pool_size) {}

void task_executor::update(time_unit /*dt*/) {
  try_finish_task();
  try_start_task();
}

void task_executor::enqueue(unique_ptr<async_task> task) {
  AssertReturnUnless(task)

  m_scheduled_task_queue.emplace(std::move(task));
}

void task_executor::try_start_task() {
  ReturnUnless(m_in_progress_tasks.size() < m_pool_size);
  ReturnIf(m_scheduled_task_queue.empty());

  auto& task = m_scheduled_task_queue.front();

  auto& task_data = m_in_progress_tasks.emplace_back();
  std::swap(task_data.m_task, task);

  std::packaged_task<void()> packaged_task{
      std::bind(&async_task::run, task_data.m_task.get())};
  task_data.m_future = packaged_task.get_future();
  task_data.m_thread = std::jthread(std::move(packaged_task));

  m_scheduled_task_queue.pop();
}

void task_executor::try_finish_task() {
  ReturnIf(m_in_progress_tasks.empty());
  auto it = m_in_progress_tasks.begin();

  while (it != m_in_progress_tasks.end()) {
    auto& task = *it;

    // check if it's ready
    if (task.m_future.wait_for(std::chrono::milliseconds(0)) !=
        std::future_status::ready) {
      ++it;
      continue;
    }

    if (task.m_task) {
      task.m_task->execute_on_main_thread();
    } else {
      AssertLogIf(true, "Task is missing")
    }

    it = m_in_progress_tasks.erase(it);
  }
}

}  // namespace wunder