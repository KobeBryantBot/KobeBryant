#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <chrono>
#include <functional>

class Schedule {
public:
    using TaskID = size_t;
    using Task   = std::function<void()>;

    /**
     * 添加一个延迟任务
     * @param duration 延迟时间
     * @param task 需要执行的任务
     * @return 任务ID
     */
    template <class T, class D>
    static inline TaskID addDelayTask(std::chrono::duration<T, D> duration, Task&& task) {
        return addDelay(
            utils::getCurrentPluginName(),
            std::chrono::duration_cast<std::chrono::milliseconds>(duration),
            std::move(task)
        );
    }

    /**
     * 添加一个重复任务
     * @param duration 重复时间
     * @param task 需要执行的任务
     * @return 任务ID
     */
    template <class T, class D>
    static inline TaskID addRepeatTask(std::chrono::duration<T, D> duration, Task&& task, bool immediately = false) {
        return addRepeat(
            utils::getCurrentPluginName(),
            std::chrono::duration_cast<std::chrono::milliseconds>(duration),
            std::move(task),
            immediately
        );
    }

    /**
     * 添加一个重复任务
     * @param duration 重复时间
     * @param task 需要执行的任务
     * @param times 任务重复次数
     * @return 任务ID
     */
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

    /**
     * 添加一个条件任务
     * @param task 需要执行的任务
     * @param condition 任务执行条件
     * @return 任务ID
     */
    static inline TaskID addConditionTask(Task&& task, std::function<bool()>&& condition) {
        return addCondition(utils::getCurrentPluginName(), std::move(task), std::move(condition));
    }

    /**
     * 添加一个条件任务
     * @param task 需要执行的任务
     * @param condition 任务执行条件
     * @param times 任务重复次数
     * @return 任务ID
     */
    static inline TaskID addConditionTask(Task&& task, std::function<bool()>&& condition, size_t times) {
        return addCondition(utils::getCurrentPluginName(), std::move(task), std::move(condition), times);
    }

    /**
     * 添加一个cron任务
     * @param cron 时间表达式
     * @param task 需要执行的任务
     * @return 任务ID
     */
    static inline TaskID addCronTask(const std::string& cron, Task&& task) {
        return addCron(utils::getCurrentPluginName(), cron, std::move(task));
    }

    /**
     * 添加一个cron任务
     * @param cron 时间表达式
     * @param task 需要执行的任务
     * @param times 任务重复次数
     * @return 任务ID
     */
    static inline TaskID addCronTask(const std::string& cron, Task&& task, size_t times) {
        return addCron(utils::getCurrentPluginName(), cron, std::move(task), times);
    }

    /**
     * 取消一个任务
     * @param id 任务ID
     * @return 取消成功返回 true，否则返回 false
     */
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