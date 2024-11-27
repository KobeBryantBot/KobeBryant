#include "api/Schedule.hpp"
#include "core/modules/ScheduleManager.hpp"

Schedule::TaskID Schedule::addDelay(const std::string& plugin, std::chrono::milliseconds delay, Task&& task) {
    return ScheduleManager::getInstance().addDelayTask(plugin, delay, std::move(task));
}

Schedule::TaskID
Schedule::addRepeat(const std::string& plugin, std::chrono::milliseconds interval, Task&& task, bool immediately) {
    if (immediately && task) {
        task();
    }
    return ScheduleManager::getInstance().addRepeatTask(plugin, interval, std::move(task));
}

Schedule::TaskID Schedule::addRepeat(
    const std::string&        plugin,
    std::chrono::milliseconds interval,
    Task&&                    task,
    bool                      immediately,
    size_t                    times
) {
    if (immediately && task) {
        task();
        times--;
    }
    if (times >= 1) {
        return ScheduleManager::getInstance().addRepeatTask(plugin, interval, std::move(task), times);
    }
    return -1;
}

Schedule::TaskID Schedule::addCondition(const std::string& plugin, Task&& task, std::function<bool()>&& condition) {
    return ScheduleManager::getInstance().addConditionTask(plugin, std::move(task), std::move(condition));
}

Schedule::TaskID
Schedule::addCondition(const std::string& plugin, Task&& task, std::function<bool()>&& condition, size_t times) {
    return ScheduleManager::getInstance().addConditionTask(plugin, std::move(task), std::move(condition), times);
}

Schedule::TaskID Schedule::addCron(const std::string& plugin, const std::string& cron, Schedule::Task&& task) {
    return ScheduleManager::getInstance().addCronTask(plugin, cron, std::move(task));
}

Schedule::TaskID
Schedule::addCron(const std::string& plugin, const std::string& cron, Schedule::Task&& task, size_t times) {
    return ScheduleManager::getInstance().addCronTask(plugin, cron, std::move(task), times);
}

bool Schedule::cancel(const std::string& plugin, TaskID id) {
    return ScheduleManager::getInstance().cancelTask(plugin, id);
}