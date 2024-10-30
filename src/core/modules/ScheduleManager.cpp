#include "ScheduleManager.hpp"

ScheduleManager& ScheduleManager::getInstance() {
    static std::unique_ptr<ScheduleManager> instance;
    if (!instance) {
        instance = std::make_unique<ScheduleManager>();
    }
    return *instance;
}

HMODULE ScheduleManager::getTaskOwner(size_t id) {
    if (mTaskIdMap.contains(id)) {
        return mTaskIdMap[id];
    }
    return NULL;
}

size_t
ScheduleManager::addDelayTask(HMODULE hModule, std::chrono::milliseconds delay, std::function<void()> const& task) {
    auto id = KobeBryant::getInstance().addDelayTask(delay, task);
    mPluginTasks[hModule].insert(id);
    mTaskIdMap[id] = hModule;
    return id;
}

size_t
ScheduleManager::addRepeatTask(HMODULE hModule, std::chrono::milliseconds interval, std::function<void()> const& task) {
    auto id = KobeBryant::getInstance().addRepeatTask(interval, task);
    mPluginTasks[hModule].insert(id);
    mTaskIdMap[id] = hModule;
    return id;
}

size_t ScheduleManager::addRepeatTask(
    HMODULE                      hModule,
    std::chrono::milliseconds    interval,
    std::function<void()> const& task,
    uint64_t                     times
) {

    size_t id      = KobeBryant::getInstance().addRepeatTask(interval, [=] {
        if (task) {
            task();
        }
        mTaskTimes[id]--;
        if (mTaskTimes[id] <= 0) {
            ScheduleManager::getInstance().cancelTask(id);
        }
    });
    mTaskTimes[id] = times;
    mPluginTasks[hModule].insert(id);
    mTaskIdMap[id] = hModule;
    return id;
}

bool ScheduleManager::cancelTask(size_t id) {
    if (auto hModule = getTaskOwner(id)) {
        mPluginTasks[hModule].erase(id);
        mTaskIdMap.erase(id);
        mTaskTimes.erase(id);
    }
    return KobeBryant::getInstance().cancelTask(id);
}

void ScheduleManager::removePluginTasks(HMODULE hModule) {
    for (auto& id : mPluginTasks[hModule]) {
        cancelTask(id);
    }
    mPluginTasks.erase(hModule);
}

void ScheduleManager::removeAllTasks() {
    for (auto& [id, hMoudle] : mTaskIdMap) {
        cancelTask(id);
    }
    mTaskIdMap.clear();
    mPluginTasks.clear();
    mTaskTimes.clear();
}