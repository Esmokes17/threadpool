#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#define __cplusplus17 201703L
#if __cplusplus17 > __cplusplus
#    error You use old version of c++, please use c++17 or higher
#endif

#ifndef __unix__
#    error now just run in unix os
#endif

#include <condition_variable>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "thread_queue.hpp"

static const bool debug = true;

#define open_one_gate(gate, task)                   \
    {                                               \
        std::unique_lock<std::mutex> lock(mutex_);  \
        task;                                       \
        gate.condition.notify_one();                \
    }

struct gate {
    mutable std::mutex mutex_;
    mutable std::condition_variable condition;
};

class ThreadPool{
private:
    int size;
    ThreadQueue queue;
    std::vector<std::thread> workers;
    mutable std::mutex mutex_;
    mutable std::condition_variable condition;
    bool stop_, join_;
    public:
    ThreadPool(int size_ = 1) {
        size = size_;
        init();
    }

    void init() {
        stop_ = false;
        join_ = false;
        for (int i = 0 ; i < size ; i++) {
            workers.emplace_back(std::thread(&ThreadPool::loop, this));
        }
    }

    void loop() {
        while (true) {
            worker task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition.wait(lock,
                    [this, &task]{
                        return (this->stop_ || this->join_ || !queue.empty());
                    }
                );
                if (join_ && (this->stop_ || queue.empty())) {
                    return;
                }
                if (!queue.pop(task) && debug) {
                    throw std::runtime_error("There is no tasks to do");
                }
            }
            if (task != NULL)
                task();
        }
    }

    template <class F, class ...Args>
    void addTask(F&& callable, Args&& ...args) {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto bind = std::bind(std::forward<F>(callable), std::forward<Args>(args)...);
        std::shared_ptr<std::packaged_task<return_type()>> task =
            std::make_shared<std::packaged_task<return_type()>>(std::move(bind));
        std::future<return_type> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(stop_)
                throw std::runtime_error("Running tasks was stopped");
            queue.emplace([task]() { (*task)(); });
        }
    }

    template <class F, class ...Args>
    auto execute(F&& callable, Args&& ...args)
        -> std::future<typename std::result_of<F(Args...)>::type>{
        using return_type = typename std::result_of<F(Args...)>::type;
        auto bind = std::bind(std::forward<F>(callable), std::forward<Args>(args)...);
        std::shared_ptr<std::packaged_task<return_type()>> task =
            std::make_shared<std::packaged_task<return_type()>>(std::move(bind));
        std::future<return_type> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(stop_)
                throw std::runtime_error("Running tasks was stopped");
            queue.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return result;
    }

    void executeAll() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition.notify_all();
    }

    void terminate() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        condition.notify_all();
        for (std::thread& active_thread : workers) {
            active_thread.join();
        }
        workers.clear();
    }


    void join() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            join_ = true;
        }
        condition.notify_all();
        for (std::thread& active_thread : workers) {
            active_thread.join();
        }
        workers.clear();
    }


    ~ThreadPool() {
        terminate();
    }
};

#endif // THREADPOOL_HPP
