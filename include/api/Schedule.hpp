#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <chrono>
#include <functional>

class Schedule {
public:
    using TaskID = size_t;
    using Task   = std::function<void()>;

    template <class T, class D>
    static inline TaskID addDelayTask(std::chrono::duration<T, D> duration, Task&& task) {
        return addDelay(
            utils::getCurrentPluginName(),
            std::chrono::duration_cast<std::chrono::milliseconds>(duration),
            std::move(task)
        );
    }

    template <class T, class D>
    static inline TaskID addRepeatTask(std::chrono::duration<T, D> duration, Task&& task, bool immediately = false) {
        return addRepeat(
            utils::getCurrentPluginName(),
            std::chrono::duration_cast<std::chrono::milliseconds>(duration),
            std::move(task),
            immediately
        );
    }

    template <class T, class D>
    static inline TaskID
    addRepeatTask(std::chrono::duration<T, D> duration, Task&& task, bool immediately, size_t times) {
        return addRepeat(
            utils::getCurrentPluginName(),
            std::chrono::duration_cast<std::chrono::milliseconds>(duration),
            std::move(task),
            immediately,
            times
        );
    }

    static inline TaskID addConditionTask(Task&& task, std::function<bool()>&& condition) {
        return addCondition(utils::getCurrentPluginName(), std::move(task), std::move(condition));
    }

    static inline TaskID addConditionTask(Task&& task, std::function<bool()>&& condition, size_t times) {
        return addCondition(utils::getCurrentPluginName(), std::move(task), std::move(condition), times);
    }

    static inline TaskID addCronTask(const std::string& cron, Task&& task) {
        return addCron(utils::getCurrentPluginName(), cron, std::move(task));
    }

    static inline TaskID addCronTask(const std::string& cron, Task&& task, size_t times) {
        return addCron(utils::getCurrentPluginName(), cron, std::move(task), times);
    }

    static inline bool cancelTask(TaskID id) { return cancel(utils::getCurrentPluginName(), id); }

protected:
    KobeBryant_NDAPI static TaskID addDelay(const std::string&, std::chrono::milliseconds, Task&&);
    KobeBryant_NDAPI static TaskID addRepeat(const std::string&, std::chrono::milliseconds, Task&&, bool);
    KobeBryant_NDAPI static TaskID addRepeat(const std::string&, std::chrono::milliseconds, Task&&, bool, size_t);
    KobeBryant_NDAPI static TaskID addCondition(const std::string&, Task&&, std::function<bool()>&&);
    KobeBryant_NDAPI static TaskID addCondition(const std::string&, Task&&, std::function<bool()>&&, size_t);
    KobeBryant_NDAPI static TaskID addCron(const std::string&, const std::string&, Task&&);
    KobeBryant_NDAPI static TaskID addCron(const std::string&, const std::string&, Task&&, size_t);
    KobeBryant_NDAPI static bool   cancel(const std::string&, TaskID);
};