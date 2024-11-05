#pragma once
#include "concurrentqueue/concurrentqueue.h"
#include <atomic>
#include <functional>
#include <semaphore>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
#include <xutility>

struct default_task_invoke {
    template <class F>
    constexpr void operator()(F&& f) const {
        std::invoke(std::forward<F>(f));
    }
};

template <class Task = std::function<void()>, class Init = std::identity, class Invoke = default_task_invoke>
class ThreadPool {
    using queue_type = moodycamel::ConcurrentQueue<Task>;
    using token_type = typename queue_type::consumer_token_t;

protected:
    std::vector<std::thread>  mWorkers;
    queue_type                mTasks;
    std::counting_semaphore<> mSemaphore{0};
    std::atomic_bool          mStop{false};
    Init                      mIniter;
    Invoke                    mInvoker;

public:
    ThreadPool(size_t threads)
        requires(std::is_default_constructible_v<Init> && std::is_default_constructible_v<Invoke>)
    : ThreadPool(threads, Init{}, Invoke{}) {}

    ThreadPool(size_t threads, Init init)
        requires(std::is_default_constructible_v<Invoke>)
    : ThreadPool(threads, std::move(init), Invoke{}) {}

    ThreadPool(size_t threads, Init init, Invoke invoke) : mIniter(std::move(init)), mInvoker(std::move(invoke)) {
        mWorkers.reserve(threads);
        for (size_t i = 0; i < threads; ++i) {
            mWorkers.emplace_back([this, i] {
                (void)mIniter(i);
                token_type token{mTasks};
                for (;;) {
                    Task task;
                    mSemaphore.acquire();
                    while (!mTasks.try_dequeue(token, task)) {
                        if (mStop.load(std::memory_order_relaxed)) {
                            return;
                        }
                    }
                    mInvoker(task);
                }
            });
        }
    }

    void enqueue(Task&& task) {
        mTasks.enqueue(std::move(task));
        mSemaphore.release();
    }

    ~ThreadPool() {
        mStop = true;
        mSemaphore.release(mWorkers.size());
        for (auto& worker : mWorkers) {
            if (worker.joinable()) worker.join();
        }
    }
};
