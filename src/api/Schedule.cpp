#include "api/Schedule.hpp"
#include "core/modules/ScheduleManager.hpp"

Schedule::TaskID Schedule::addDelay(std::string const& plugin, std::chrono::milliseconds delay, Task const& task) {
    return ScheduleManager::getInstance().addDelayTask(plugin, delay, task);
}

Schedule::TaskID Schedule::addRepeat(std::string const& plugin, std::chrono::milliseconds delay, Task const& task) {
    return ScheduleManager::getInstance().addRepeatTask(plugin, delay, task);
}

Schedule::TaskID
Schedule::addRepeat(std::string const& plugin, std::chrono::milliseconds delay, Task const& task, uint64_t times) {
    return ScheduleManager::getInstance().addRepeatTask(plugin, delay, task, times);
}

bool Schedule::cancelTask(TaskID id) { return ScheduleManager::getInstance().cancelTask(id); }