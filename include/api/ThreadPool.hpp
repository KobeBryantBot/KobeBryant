#pragma once
#include "Macros.hpp"
#include <functional>
#include <iostream>

class ThreadPool {
    class ThreadPoolImpl;
    std::unique_ptr<ThreadPoolImpl> mImpl;

public:
    using Task = std::function<void()>;

    KobeBryant_NDAPI explicit ThreadPool(size_t threads);
    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    KobeBryant_API void enqueue(Task&& task);
};
