#pragma once
#include "Macros.hpp"
#include <functional>
#include <iostream>

class ThreadPool {
    class ThreadPoolImpl;
    std::unique_ptr<ThreadPoolImpl> mImpl;

public:
    using Task = std::function<void()>;

    /**
     * @brief 构造函数
     * @param threads 线程池中的线程数量
     */
    KobeBryant_NDAPI explicit ThreadPool(size_t threads);
    ThreadPool(const ThreadPool&)            = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    /**
     * @brief 添加一个任务到线程池
     * @param task 任务
     */
    KobeBryant_API void enqueue(Task&& task);
};
