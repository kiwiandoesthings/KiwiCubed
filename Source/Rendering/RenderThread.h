#pragma once

#include <klogger.hpp>
#include <GLError.h>

#include <functional>
#include <mutex>
#include <queue>
#include <vector>


class RenderThread {
    public:
        static RenderThread& GetInstance();
        
        void Delete();

        void AddTask(std::function<void()> task);

        void Run();

    private:
        RenderThread() = default;
        ~RenderThread() = default;

        RenderThread(const RenderThread&) = delete;
        RenderThread& operator=(const RenderThread&) = delete;

        std::mutex mutex;

        bool flushing = false;

        std::vector<std::function<void()>> backBufferTasks;
        std::vector<std::function<void()>> frontBufferTasks;

        void SwapBuffers();
};