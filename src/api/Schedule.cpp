#include "api/Schedule.hpp"
#include "core/modules/ScheduleManager.hpp"

Schedule::TaskID Schedule::addDelay(const std::string& plugin, std::chrono::milliseconds delay, Task&& task) {
    return ScheduleManager::getInstance().addDelayTask(plugin, delay, std::move(task));
}

Schedule::TaskID
Schedule::addRepeat(const std::string& plugin, std::chrono::milliseconds delay, Task&& task, bool immediately) {
    if (immediately && task) {
        task();
    }
    return ScheduleManager::getInstance().addRepeatTask(plugin, delay, std::move(task));
}

Schedule::TaskID Schedule::addRepeat(
    const std::string&        plugin,
    std::chrono::milliseconds delay,
    Task&&                    task,
    bool                      immediately,
    uint64_t                  times
) {
    if (immediately && task) {
        task();
        times--;
    }
    if (times >= 1) {
        return ScheduleManager::getInstance().addRepeatTask(plugin, delay, std::move(task), times);
    }
    return -1;
}

bool Schedule::cancel(const std::string& plugin, TaskID id) {
    return ScheduleManager::getInstance().cancelTask(plugin, id);
}