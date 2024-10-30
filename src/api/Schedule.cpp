#include "api/Schedule.hpp"
#include "core/modules/KobeBryant.hpp"

Schedule::TaskID Schedule::addDelay(std::chrono::milliseconds delay, Task const& task) {
    return KobeBryant::getInstance().addDelayTask(delay, task);
}

Schedule::TaskID Schedule::addRepeat(std::chrono::milliseconds delay, Task const& task) {
    return KobeBryant::getInstance().addRepeatTask(delay, task);
}

bool Schedule::cancelTask(TaskID id) { return KobeBryant::getInstance().cancelTask(id); }