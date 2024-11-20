#include "api/Schedule.hpp"
#include "core/modules/ScheduleManager.hpp"

Schedule::TaskID Schedule::addDelay(const std::string& plugin, std::chrono::milliseconds delay, const Task& task) {
    return ScheduleManager::getInstance().addDelayTask(plugin, delay, task);
}

Schedule::TaskID Schedule::addRepeat(const std::string& plugin, std::chrono::milliseconds delay, const Task& task) {
    return ScheduleManager::getInstance().addRepeatTask(plugin, delay, task);
}

Schedule::TaskID
Schedule::addRepeat(const std::string& plugin, std::chrono::milliseconds delay, const Task& task, uint64_t times) {
    return ScheduleManager::getInstance().addRepeatTask(plugin, delay, task, times);
}

bool Schedule::cancel(const std::string& plugin, TaskID id) {
    return ScheduleManager::getInstance().cancelTask(plugin, id);
}