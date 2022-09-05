#ifndef THREAD_QUEUE_HPP
#define THREAD_QUEUE_HPP

#include <functional>
#include <mutex>
#include <queue>
#include <shared_mutex>

typedef std::function<void()> worker;

class ThreadQueue{
private:
    std::queue<worker> tasks;
    mutable std::shared_mutex mutex_;
public:
    ThreadQueue() = default;

    void push(worker& worker_) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        tasks.push(worker_);
    }

    void emplace(worker&& worker_) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        tasks.emplace(worker_);
    }

    bool pop(worker& holder) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (tasks.empty()) {
            return false;
        }

        holder = std::move(tasks.front());
        tasks.pop();
        return true;
    }

    bool empty() { return tasks.empty(); }
};

#endif // THREAD_QUEUE_HPP
