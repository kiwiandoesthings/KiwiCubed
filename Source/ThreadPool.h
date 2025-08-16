#include <klogger.hpp>
#include <debug-trap.h>

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class ThreadPool {
    public:
        ThreadPool(unsigned int threadCount = std::thread::hardware_concurrency());
        void Delete();

        void QueueTask(std::function<void()> task) {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                tasks.emplace(std::move(task));
            }
            conditionVariable.notify_one();
        }

        unsigned int GetQueueSize();

    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()> > tasks;
        std::atomic<int> activeTasks = 0;
        std::mutex queueMutex;
        std::condition_variable conditionVariable;
        bool stop = false;

        unsigned int maximumQueuedTasks = 10;
};