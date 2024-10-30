#pragma once
#include "KobeBryant.hpp"
#include <unordered_set>

class ScheduleManager {
private:
    std::unordered_map<HMODULE, std::unordered_set<size_t>> mPluginTasks;
    std::unordered_map<size_t, HMODULE>                     mTaskIdMap;

public:
    static ScheduleManager& getInstance();

    HMODULE getTaskOwner(size_t id);

    size_t addDelayTask(HMODULE hModule, std::chrono::milliseconds delay, std::function<void()> const& task);

    size_t addRepeatTask(HMODULE hModule, std::chrono::milliseconds interval, std::function<void()> const& task);

    bool cancelTask(size_t id);

    void removePluginTasks(HMODULE hModule);

    void removeAllTasks();
};
