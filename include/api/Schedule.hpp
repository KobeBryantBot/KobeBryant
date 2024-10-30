#pragma once
#include "Macros.hpp"
#include <chrono>
#include <functional>

class Schedule {
public:
    using TaskID = size_t;
    using Task   = std::function<void()>;

protected:
    KobeBryant_NDAPI static TaskID addDelay(std::chrono::milliseconds, Task const& task);
    KobeBryant_NDAPI static TaskID addRepeat(std::chrono::milliseconds, Task const& task);

public:
    template <class T, class D>
    static inline TaskID addDelayTask(std::chrono::duration<T, D> duration, Task const& task) {
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        return addDelay(time, task);
    }

    template <class T, class D>
    static inline TaskID addRepeatTask(std::chrono::duration<T, D> duration, Task const& task) {
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        return addRepeat(time, task);
    }

    KobeBryant_API static bool cancelTask(TaskID id);
};