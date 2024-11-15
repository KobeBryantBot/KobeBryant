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
    KobeBryant_NDAPI static TaskID addDelay(std::string const&, std::chrono::milliseconds, Task const&);
    KobeBryant_NDAPI static TaskID addRepeat(std::string const&, std::chrono::milliseconds, Task const&);
    KobeBryant_NDAPI static TaskID addRepeat(std::string const&, std::chrono::milliseconds, Task const&, uint64_t);
    KobeBryant_NDAPI static bool   cancel(std::string const&, TaskID);

public:
    template <class T, class D>
    static inline TaskID addDelayTask(std::chrono::duration<T, D> duration, Task const& task) {
        auto time   = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        auto plugin = utils::getCurrentPluginName();
        return addDelay(plugin, time, task);
    }

    template <class T, class D>
    static inline TaskID
    addRepeatTask(std::chrono::duration<T, D> duration, Task const& task, bool immediately = false) {
        if (immediately && task) {
            task();
        }
        auto time   = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        auto plugin = utils::getCurrentPluginName();
        return addRepeat(plugin, time, task);
    }

    template <class T, class D>
    static inline TaskID
    addRepeatTask(std::chrono::duration<T, D> duration, Task const& task, bool immediately, uint64_t times) {
        if (immediately && task) {
            task();
            times--;
        }
        if (times >= 1) {
            auto time   = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
            auto plugin = utils::getCurrentPluginName();
            return addRepeat(plugin, time, task, times);
        }
        return -1;
    }

    static inline bool cancelTask(TaskID id) {
        auto plugin = utils::getCurrentPluginName();
        return cancel(plugin, id);
    }
};