#include "api/Schedule.hpp"
#include "core/modules/ScheduleManager.hpp"

Schedule::TaskID Schedule::addDelay(HMODULE hModule, std::chrono::milliseconds delay, Task const& task) {
    return ScheduleManager::getInstance().addDelayTask(hModule, delay, task);
}

Schedule::TaskID Schedule::addRepeat(HMODULE hModule, std::chrono::milliseconds delay, Task const& task) {
    return ScheduleManager::getInstance().addRepeatTask(hModule, delay, task);
}

Schedule::TaskID
Schedule::addRepeat(HMODULE hModule, std::chrono::milliseconds delay, Task const& task, uint64_t times) {
    return ScheduleManager::getInstance().addRepeatTask(hModule, delay, task, times);
}

bool Schedule::cancelTask(TaskID id) { return ScheduleManager::getInstance().cancelTask(id); }