#include "ThreadPool.h"


ThreadPool::ThreadPool(unsigned int threadCount) {
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);

                    conditionVariable.wait(lock, [this] {
                        return !tasks.empty() || stop;
                    });

                    if (stop && tasks.empty()) {
                        return;
                    }

                    task = std::move(tasks.front());
                    tasks.pop();
                }
                
                activeTasks++;
                task();
                activeTasks--;
            }
        });
    }
}

void ThreadPool::Delete() {
    OVERRIDE_LOG_NAME("Thread Pool Destruction");
    if (activeTasks.load() > 0) {
        WARN("Thread pool still running tasks at shutdown, shutdown might fail");
    }

    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    conditionVariable.notify_all();

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        } else {
            WARN("Thread in thread pool was not joinable (could not be stopped)");
        }
    }

    INFO("Destroyed thread pool");
}

unsigned int ThreadPool::GetQueueSize() {
    return static_cast<unsigned int>(tasks.size());
}