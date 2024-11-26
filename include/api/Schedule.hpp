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
    KobeBryant_NDAPI static TaskID addDelay(const std::string&, std::chrono::milliseconds, Task&&);
    KobeBryant_NDAPI static TaskID addRepeat(const std::string&, std::chrono::milliseconds, Task&&, bool);
    KobeBryant_NDAPI static TaskID addRepeat(const std::string&, std::chrono::milliseconds, Task&&, bool, size_t);
    KobeBryant_NDAPI static TaskID addCondition(const std::string&, Task&&, std::function<bool()>&&);
    KobeBryant_NDAPI static TaskID addCondition(const std::string&, Task&&, std::function<bool()>&&, size_t);
    KobeBryant_NDAPI static bool   cancel(const std::string&, TaskID);

public:
    template <class T, class D>
    static inline TaskID addDelayTask(std::chrono::duration<T, D> duration, Task&& task) {
        auto time   = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        auto plugin = utils::getCurrentPluginName();
        return addDelay(plugin, time, std::move(task));
    }

    template <class T, class D>
    static inline TaskID addRepeatTask(std::chrono::duration<T, D> duration, Task&& task, bool immediately = false) {
        auto time   = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        auto plugin = utils::getCurrentPluginName();
        return addRepeat(plugin, time, std::move(task), immediately);
    }

    template <class T, class D>
    static inline TaskID
    addRepeatTask(std::chrono::duration<T, D> duration, Task&& task, bool immediately, size_t times) {
        auto time   = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        auto plugin = utils::getCurrentPluginName();
        return addRepeat(plugin, time, std::move(task), immediately, times);
    }

    static inline TaskID addConditionTask(Task&& task, std::function<bool()>&& condition) {
        auto plugin = utils::getCurrentPluginName();
        return addCondition(plugin, std::move(task), std::move(condition));
    }

    static inline TaskID addConditionTask(Task&& task, std::function<bool()>&& condition, size_t times) {
        auto plugin = utils::getCurrentPluginName();
        return addCondition(plugin, std::move(task), std::move(condition), times);
    }

    static inline bool cancelTask(TaskID id) {
        auto plugin = utils::getCurrentPluginName();
        return cancel(plugin, id);
    }
};