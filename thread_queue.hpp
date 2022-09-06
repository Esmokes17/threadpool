#ifndef THREAD_QUEUE_HPP
#define THREAD_QUEUE_HPP


#define __cplusplus17 201703L
#if __cplusplus17 > __cplusplus
#    error You use old version of c++, please use c++17 or higher
#endif

#ifndef __unix__
#    error Now just runnable in unix os
#endif

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

    inline void push(worker& worker_) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        tasks.push(worker_);
    }

    inline void emplace(worker&& worker_) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        tasks.emplace(worker_);
    }

    inline bool pop(worker& holder) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (tasks.empty()) {
            return false;
        }

        holder = std::move(tasks.front());
        tasks.pop();
        return true;
    }

    inline bool empty() { return tasks.empty(); }
};

#endif // THREAD_QUEUE_HPP
