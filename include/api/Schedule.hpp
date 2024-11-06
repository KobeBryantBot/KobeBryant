#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <chrono>
#include <functional>

class Schedule {
public:
    using TaskID = size_t;
    using Task   = std::function<void()>;

protected:
    KobeBryant_NDAPI static TaskID addDelay(HMODULE hModule, std::chrono::milliseconds, Task const& task);
    KobeBryant_NDAPI static TaskID addRepeat(HMODULE hModule, std::chrono::milliseconds, Task const& task);
    KobeBryant_NDAPI static TaskID
    addRepeat(HMODULE hModule, std::chrono::milliseconds, Task const& task, uint64_t times);

public:
    template <class T, class D>
    static inline TaskID addDelayTask(std::chrono::duration<T, D> duration, Task const& task) {
        auto time    = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        auto hModule = utils::getCurrentModuleHandle();
        return addDelay(hModule, time, task);
    }

    template <class T, class D>
    static inline TaskID
    addRepeatTask(std::chrono::duration<T, D> duration, Task const& task, bool immediately = false) {
        if (immediately && task) {
            task();
        }
        auto time    = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        auto hModule = utils::getCurrentModuleHandle();
        return addRepeat(hModule, time, task);
    }

    template <class T, class D>
    static inline TaskID
    addRepeatTask(std::chrono::duration<T, D> duration, Task const& task, bool immediately, uint64_t times) {
        if (immediately && task) {
            task();
            times--;
        }
        if (times >= 1) {
            auto time    = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
            auto hModule = utils::getCurrentModuleHandle();
            return addRepeat(hModule, time, task, times);
        }
        return -1;
    }

    KobeBryant_API static bool cancelTask(TaskID id);
};