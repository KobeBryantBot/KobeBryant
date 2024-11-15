#pragma once
#include "KobeBryant.hpp"
#include <unordered_set>

class Scheduler {
public:
    using TaskID = size_t;
    using Task   = std::function<void()>;

private:
    struct TaskInfo {
        TaskID                                id;
        Task                                  task;
        std::chrono::steady_clock::time_point runTime;
        std::chrono::milliseconds             interval;
        std::atomic<bool>                     cancelled{false};

        TaskInfo(
            TaskID                                id,
            Task                                  task,
            std::chrono::steady_clock::time_point runTime,
            std::chrono::milliseconds             interval
        )
        : id(id),
          task(std::move(task)),
          runTime(runTime),
          interval(interval) {}
    };

protected:
    std::mutex                             mMtx;
    std::condition_variable                mCv;
    std::vector<std::shared_ptr<TaskInfo>> mTasks;
    std::unordered_map<TaskID, size_t>     mTaskIndexMap;
    TaskID                                 mNextTaskID = 0;

public:
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

    bool cancelTask(size_t id);

    void removePluginTasks(std::string const& plugin);

    void removeAllTasks();
};
