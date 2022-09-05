#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <future>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

#include "thread_queue.hpp"

class ThreadPool{
private:
    int size;
    ThreadQueue queue;
    std::vector<std::thread> workers;
    mutable std::mutex mutex_;
    std::condition_variable condition;
    bool stop;
    public:
    ThreadPool(int size_ = 1) {
        size = size_;
        init();
    }

    void init() {
        stop = false;
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
                    [this]{ return (this->stop || !queue.empty());});
                if (this->stop || queue.empty()) {
                    return;
                }
                if(!queue.pop(task)) {
                    throw std::runtime_error("Bad pop");
                }
            }
            task();
        }
    }

    template <class F, class ...Args>
    auto execute(F&& callable, Args&& ...args)
        -> std::future<typename std::result_of<F(Args...)>::type>{
        using return_type = typename std::result_of<F(Args...)>::type;
        auto bind = std::bind(std::forward<F>(callable), std::forward<Args>(args)...);
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::move(bind));
        std::future<return_type> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            queue.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return result;
    }

    void terminate() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop = true;
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
