#pragma once
#include "KobeBryant.hpp"
#include "croncpp/croncpp.h"
#include <unordered_set>

class Scheduler {
public:
    using TaskID = size_t;
    using Task   = std::function<void()>;

private:
    struct TaskInfo {
        Task                                  mTask;
        std::chrono::steady_clock::time_point mRunTime;
        std::chrono::milliseconds             mInterval;

        TaskInfo(const Task& task, std::chrono::steady_clock::time_point runTime, std::chrono::milliseconds interval)
        : mTask(std::move(task)),
          mRunTime(runTime),
          mInterval(interval) {}
    };

protected:
    std::mutex                                  mMtx;
    std::condition_variable                     mCv;
    std::map<TaskID, std::unique_ptr<TaskInfo>> mTasks;
    TaskID                                      mNextTaskID = 0;

public:
    Scheduler() = default;

    ~Scheduler();

    static Scheduler& getInstance();

    TaskID addDelayTask(std::chrono::milliseconds delay, const Task& task);

    TaskID addRepeatTask(std::chrono::milliseconds delay, const Task& task);

    bool cancelTask(TaskID id);
};

class ScheduleManager {
private:
    std::unordered_map<std::string, std::unordered_set<size_t>> mPluginTasks;
    std::unordered_map<size_t, std::string>                     mTaskIdMap;
    std::unordered_map<size_t, uint64_t>                        mTaskTimes;
    std::unordered_map<size_t, time_t>                          mCornTime;

public:
    static ScheduleManager& getInstance();

    std::string getTaskOwner(size_t id);

    size_t addDelayTask(const std::string& plugin, std::chrono::milliseconds delay, const std::function<void()>& task);

    size_t
    addRepeatTask(const std::string& plugin, std::chrono::milliseconds interval, const std::function<void()>& task);

    size_t addRepeatTask(
        const std::string&           plugin,
        std::chrono::milliseconds    interval,
        const std::function<void()>& task,
        uint64_t                     times
    );

    size_t addConditionTask(
        const std::string&           plugin,
        const std::function<void()>& task,
        const std::function<bool()>& condition
    );

    size_t addConditionTask(
        const std::string&           plugin,
        const std::function<void()>& task,
        const std::function<bool()>& condition,
        size_t                       times
    );

    size_t addCronTask(const std::string& plugin, const std::string& cron, const std::function<void()>& task);

    size_t
    addCronTask(const std::string& plugin, const std::string& cron, const std::function<void()>& task, size_t times);

    bool cancelTask(const std::string& plugin, size_t id);

    void removePluginTasks(const std::string& plugin);

    void removeAllTasks();
};
