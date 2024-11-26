#include "api/ThreadPool.hpp"
#include "core/modules/ThreadPoolBase.hpp"

class ThreadPool::ThreadPoolImpl {
    ThreadPoolBase<> mPool;

public:
    explicit ThreadPoolImpl(size_t threads) : mPool(threads) {}

    void enqueue(ThreadPool::Task&& task) { mPool.enqueue(std::move(task)); }
};

ThreadPool::ThreadPool(size_t threads) : mImpl(std::make_unique<ThreadPoolImpl>(threads)) {}

void ThreadPool::enqueue(ThreadPool::Task&& task) { mImpl->enqueue(std::move(task)); }