#pragma once
#include "KobeBryant.hpp"
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
        std::atomic<bool>                     mCancelled{false};

        TaskInfo(Task task, std::chrono::steady_clock::time_point runTime, std::chrono::milliseconds interval)
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

    TaskID addDelayTask(std::chrono::milliseconds delay, Task const& task);

    TaskID addRepeatTask(std::chrono::milliseconds delay, Task const& task);

    bool cancelTask(TaskID id);
};

class ScheduleManager {
private:
    std::unordered_map<std::string, std::unordered_set<size_t>> mPluginTasks;
    std::unordered_map<size_t, std::string>                     mTaskIdMap;
    std::unordered_map<size_t, uint64_t>                        mTaskTimes;

public:
    static ScheduleManager& getInstance();

    std::string getTaskOwner(size_t id);

    size_t addDelayTask(std::string const& plugin, std::chrono::milliseconds delay, std::function<void()> const& task);

    size_t
    addRepeatTask(std::string const& plugin, std::chrono::milliseconds interval, std::function<void()> const& task);

    size_t addRepeatTask(
        std::string const&           plugin,
        std::chrono::milliseconds    interval,
        std::function<void()> const& task,
        uint64_t                     times
    );

    bool cancelTask(std::string const& plugin, size_t id);

    void removePluginTasks(std::string const& plugin);

    void removeAllTasks();
};
