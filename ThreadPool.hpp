//
// Created by Administrator on 24-6-22.
//

#ifndef ZR_CORE_THREADPOOL_HPP
#define ZR_CORE_THREADPOOL_HPP

#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <future>
#include <vector>
#include <queue>

namespace Utils {
    class ThreadPool {
    public:
        ThreadPool(size_t numThreads) {
            for (size_t i = 0; i < numThreads; ++i) {
                workers.emplace_back([this] {
                    while (true) {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->queueMutex);
                            this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return; // stop and no more tasks
                            task = std::move(this->tasks.front()); // get the task
                            this->tasks.pop(); // remove the task
                        }
                        task(); // execute the task
                    }
                });
            }
        }

        ~ThreadPool() {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                stop = true;
            }
            condition.notify_all();
            for (std::thread &worker: workers) {
                worker.join(); // wait for all threads to finish
            }
        }

        template<typename F, typename... Args>
        auto addTask(F &&f, Args &&...args)
        -> std::future<typename std::result_of<F(Args...)>::type> {
            using ReturnType = typename std::result_of<F(Args...)>::type;

            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                    std::bind(std::forward<F>(f),
                              std::forward<Args>(args)...)); // wrap the function object into a packaged_task

            std::future<ReturnType> result = task->get_future();
            {
                std::unique_lock<std::mutex> lock(queueMutex);

                if (stop)
                    throw std::runtime_error("addTask on stopped ThreadPool");

                tasks.emplace([task]() { (*task)(); }); // enqueue the task
            }
            condition.notify_one();
            return result;
        }

    private:
        std::vector<std::thread> workers;        // need to keep track of threads so we can join them
        std::queue<std::function<void()>> tasks; // the task queue

        std::mutex queueMutex;             // synchronization
        std::condition_variable condition; // synchronization
        bool stop = false;                 // synchronization
    };
}

#endif //ZR_CORE_THREADPOOL_HPP
