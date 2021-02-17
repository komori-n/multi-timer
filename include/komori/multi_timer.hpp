#ifndef KOMORI_MULTI_TIMER_HPP_
#define KOMORI_MULTI_TIMER_HPP_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace komori {
  namespace detail {
    using system_clock = std::chrono::system_clock;
    using time_point = std::chrono::system_clock::time_point;
    using duration = std::chrono::system_clock::duration;

    template <typename Task>
    struct TaskSchedule {
      // add mutable to move with priority_queue.top()
      mutable Task task;
      time_point tp;
    };

    template <typename Task>
    struct ScheduleCompare {
      bool operator()(const TaskSchedule<Task>& x, const TaskSchedule<Task>& y) const {
        return x.tp > y.tp;
      }
    };
  }  // namespace detail

  template <typename Task=std::function<void(void)>>
  class MultiTimer {
  public:
    MultiTimer(void) : is_end_(false) {};
    MultiTimer(const MultiTimer&) = delete;
    MultiTimer(MultiTimer&&) = delete;
    MultiTimer operator=(const MultiTimer&) = delete;
    MultiTimer operator=(MultiTimer&&) = delete;

    ~MultiTimer(void) {
      is_end_ = true;
      cond_.notify_one();
      if (th_.joinable()) {
        th_.join();
      }
    }

    void start_processing(void) {
      th_ = std::thread(&MultiTimer::timerLoop, this);
    }

    void set(Task&& task, detail::duration duration) {
      std::lock_guard<std::recursive_mutex> lock(mutex_);

      auto tp = detail::system_clock::now() + duration;
      TaskSchedule schedule { std::move(task), tp };
      task_queue_.push(std::move(schedule));

      cond_.notify_one();
    }

    // enable if Task is copy-constructible
    template <std::nullptr_t Dummy = nullptr>
    auto set(const Task& task, detail::duration duration)
    -> typename std::enable_if<std::is_copy_constructible<Task>::value && Dummy == nullptr, void>::type {
      std::lock_guard<std::recursive_mutex> lock(mutex_);

      auto tp = detail::system_clock::now() + duration;
      TaskSchedule schedule { task, tp };
      task_queue_.push(std::move(schedule));

      cond_.notify_one();
    }

    bool empty(void) const {
      std::lock_guard<std::recursive_mutex> lock(mutex_);
      return task_queue_.empty();
    }

    size_t size(void) const {
      std::lock_guard<std::recursive_mutex> lock(mutex_);
      return task_queue_.size();
    }

  private:
    using TaskSchedule = detail::TaskSchedule<Task>;
    using QueueType = std::priority_queue<
      TaskSchedule,
      std::vector<TaskSchedule>,
      detail::ScheduleCompare<Task>>;

    void timerLoop(void) {
      std::unique_lock<std::recursive_mutex> lock(mutex_);

      while (!is_end_) {
        if (task_queue_.empty()) {
          cond_.wait(lock, [&](void) {
            return is_end_ || !task_queue_.empty();
          });
        } else {
          auto next_tp = task_queue_.top().tp;
          // sleep until next time point
          cond_.wait_until(lock, next_tp, [&](void) {
            auto now = detail::system_clock::now();
            return is_end_
              || (!task_queue_.empty() && next_tp <= now);
          });

          while (!is_end_ && !task_queue_.empty() &&
              task_queue_.top().tp <= detail::system_clock::now()) {
            auto task = std::move(task_queue_.top().task);
            task_queue_.pop();

            task();
          }
        }
      }
    }

    std::atomic<bool> is_end_;
    std::thread th_;

    mutable std::recursive_mutex mutex_;
    std::condition_variable_any cond_;
    QueueType task_queue_;
  };

}  // namespace komori

#endif  // KOMORI_MULTI_TIMER_HPP_
