#include "RenderThread.h"


RenderThread& RenderThread::GetInstance() {
    static RenderThread instance;
    return instance;
}

void RenderThread::AddTask(std::function<void()> task) {
    OVERRIDE_LOG_NAME("Render Thread");
    if (flushing) {
        //DEBUG("Added task during flush");
    }

    std::lock_guard<std::mutex> lock(mutex);

    backBufferTasks.emplace_back(std::move(task));
}

void RenderThread::Run() {
    flushing = true;

    SwapBuffers();

    for (std::function<void()> task : frontBufferTasks) {
        std::cout << "doing task" << std::endl;
        task();
        std::cout << "finished task" << std::endl;
    }

    frontBufferTasks.clear();

    flushing = false;
}

void RenderThread::SwapBuffers() {
    std::lock_guard<std::mutex> lock(mutex);

    std::swap(frontBufferTasks, backBufferTasks);
    backBufferTasks.clear();
}

void RenderThread::Delete() {}