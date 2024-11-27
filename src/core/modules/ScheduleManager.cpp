#include "ScheduleManager.hpp"

Scheduler::~Scheduler() { mTasks.clear(); }

Scheduler& Scheduler::getInstance() {
    static std::unique_ptr<Scheduler> instance;
    if (!instance) {
        instance = std::make_unique<Scheduler>();
        KobeBryant::getInstance().getThreadPool().enqueue([&] {
            while (EXIST_FLAG) {
                std::unique_lock<std::mutex> lock(instance->mMtx);
                instance->mCv.wait(lock, [&] { return !instance->mTasks.empty(); });
                auto              now = std::chrono::high_resolution_clock::now();
                std::vector<Task> readyTasks;
                for (auto& [id, task] : instance->mTasks) {
                    if (task->mRunTime <= now) {
                        readyTasks.push_back(task->mTask);
                        if (task->mInterval.count() > 0) {
                            task->mRunTime = now + task->mInterval;
                        } else {
                            instance->mTasks.erase(id);
                        }
                    }
                }
                lock.unlock();
                for (auto& task : readyTasks) {
                    if (task) {
                        try {
                            task();
                        }
                        CATCH
                    }
                }
            }
            instance->mCv.notify_all();
        });
    }
    return *instance;
}

void Scheduler::addDelayTask(TaskID id, std::chrono::milliseconds delay, const Task& task) {
    std::lock_guard<std::mutex> lock(mMtx);
    auto                        taskInfo = std::make_unique<TaskInfo>(
        std::move(task),
        std::chrono::steady_clock::now() + delay,
        std::chrono::milliseconds(0)
    );
    mTasks[id] = std::move(taskInfo);
    mCv.notify_one();
}

void Scheduler::addRepeatTask(TaskID id, std::chrono::milliseconds interval, const Task& task) {
    std::lock_guard<std::mutex> lock(mMtx);
    auto taskInfo = std::make_unique<TaskInfo>(std::move(task), std::chrono::steady_clock::now() + interval, interval);
    mTasks[id]    = std::move(taskInfo);
    mCv.notify_one();
}

Scheduler::TaskID Scheduler::getNextID() { return mNextTaskID++; }

bool Scheduler::cancelTask(TaskID id) {
    std::lock_guard<std::mutex> lock(mMtx);
    if (mTasks.contains(id)) {
        mTasks.erase(id);
        return true;
    }
    return false;
}

ScheduleManager& ScheduleManager::getInstance() {
    static std::unique_ptr<ScheduleManager> instance;
    if (!instance) {
        instance = std::make_unique<ScheduleManager>();
    }
    return *instance;
}

std::string ScheduleManager::getTaskOwner(size_t id) {
    if (mTaskIdMap.contains(id)) {
        return mTaskIdMap[id];
    }
    return {};
}

size_t ScheduleManager::addDelayTask(
    const std::string&           plugin,
    std::chrono::milliseconds    delay,
    const std::function<void()>& task
) {
    if (task) {
        auto id = Scheduler::getInstance().getNextID();
        mPluginTasks[plugin].insert(id);
        mTaskIdMap[id] = plugin;
        Scheduler::getInstance().addDelayTask(id, delay, task);
        return id;
    }
    return -1;
}

size_t ScheduleManager::addRepeatTask(
    const std::string&           plugin,
    std::chrono::milliseconds    interval,
    const std::function<void()>& task
) {
    if (task) {
        auto id = Scheduler::getInstance().getNextID();
        mPluginTasks[plugin].insert(id);
        mTaskIdMap[id] = plugin;
        Scheduler::getInstance().addRepeatTask(id, interval, task);
        return id;
    }
    return -1;
}

size_t ScheduleManager::addRepeatTask(
    const std::string&           plugin,
    std::chrono::milliseconds    interval,
    const std::function<void()>& task,
    uint64_t                     times
) {

    if (task) {
        size_t id      = Scheduler::getInstance().getNextID();
        mTaskTimes[id] = times;
        mPluginTasks[plugin].insert(id);
        mTaskIdMap[id] = plugin;
        Scheduler::getInstance().addRepeatTask(id, interval, [=] {
            try {
                task();
                mTaskTimes[id]--;
                if (mTaskTimes[id] <= 0) {
                    ScheduleManager::getInstance().cancelTask(plugin, id);
                }
            }
            CATCH
        });
        return id;
    }
    return -1;
}

size_t ScheduleManager::addConditionTask(
    const std::string&           plugin,
    const std::function<void()>& task,
    const std::function<bool()>& condition
) {
    if (task && condition) {
        size_t id = Scheduler::getInstance().getNextID();
        mPluginTasks[plugin].insert(id);
        mTaskIdMap[id] = plugin;
        Scheduler::getInstance().addRepeatTask(id, std::chrono::seconds(1), [=] {
            if (condition()) {
                task();
            }
        });
        return id;
    }
    return -1;
}

size_t ScheduleManager::addConditionTask(
    const std::string&           plugin,
    const std::function<void()>& task,
    const std::function<bool()>& condition,
    size_t                       times
) {
    if (task && condition) {
        size_t id = Scheduler::getInstance().getNextID();
        mPluginTasks[plugin].insert(id);
        mTaskIdMap[id] = plugin;
        mTaskTimes[id] = times;
        Scheduler::getInstance().addRepeatTask(id, std::chrono::seconds(1), [=] {
            if (condition()) {
                task();
                mTaskTimes[id]--;
                if (mTaskTimes[id] <= 0) {
                    ScheduleManager::getInstance().cancelTask(plugin, id);
                }
            }
        });
        return id;
    }
    return -1;
}

std::time_t getNextCornTime(const cron::cronexpr& cron) {
    std::time_t now = std::time(0);
    return cron::cron_next(cron, now);
}

size_t ScheduleManager::addCronTask(
    const std::string&           plugin,
    const std::string&           cron_str,
    const std::function<void()>& task
) {
    try {
        if (task) {
            auto   cron = cron::make_cron(cron_str);
            size_t id   = Scheduler::getInstance().getNextID();
            mPluginTasks[plugin].insert(id);
            mTaskIdMap[id] = plugin;
            mCornTime[id]  = getNextCornTime(cron);
            Scheduler::getInstance().addRepeatTask(id, std::chrono::seconds(1), [=] {
                if (std::time(0) == mCornTime[id]) {
                    mCornTime[id] = getNextCornTime(cron);
                    task();
                }
            });
            return id;
        }
    } catch (const std::exception& e) {
        Logger().error("C++ SEH Exception Caught!\n  Error: {}\n\n  At plugin: {}", e.what(), plugin);
    }
    CATCH_END
    return -1;
}

size_t ScheduleManager::addCronTask(
    const std::string&           plugin,
    const std::string&           cron_str,
    const std::function<void()>& task,
    size_t                       times
) {
    try {
        if (task) {
            auto   cron = cron::make_cron(cron_str);
            size_t id   = Scheduler::getInstance().getNextID();
            mPluginTasks[plugin].insert(id);
            mTaskIdMap[id] = plugin;
            mCornTime[id]  = getNextCornTime(cron);
            mTaskTimes[id] = times;
            Scheduler::getInstance().addRepeatTask(id, std::chrono::seconds(1), [=] {
                if (std::time(0) == mCornTime[id]) {
                    mCornTime[id] = getNextCornTime(cron);
                    task();
                    mTaskTimes[id]--;
                    if (mTaskTimes[id] <= 0) {
                        ScheduleManager::getInstance().cancelTask(plugin, id);
                    }
                }
            });
            return id;
        }
    } catch (const std::exception& e) {
        Logger().error("C++ SEH Exception Caught!\n  Error: {}\n\n  At plugin: {}", e.what(), plugin);
    }
    CATCH_END
    return -1;
}

bool ScheduleManager::cancelTask(const std::string& owner, size_t id) {
    auto plugin = getTaskOwner(id);
    if (owner == plugin) {
        mPluginTasks[plugin].erase(id);
        mTaskIdMap.erase(id);
        mTaskTimes.erase(id);
        mCornTime.erase(id);
        return Scheduler::getInstance().cancelTask(id);
    }
    return false;
}

void ScheduleManager::removePluginTasks(const std::string& plugin) {
    for (auto& id : mPluginTasks[plugin]) {
        cancelTask(plugin, id);
    }
    mPluginTasks.erase(plugin);
}

void ScheduleManager::removeAllTasks() {
    for (auto& [id, plugin] : mTaskIdMap) {
        cancelTask(plugin, id);
    }
    mTaskIdMap.clear();
    mPluginTasks.clear();
    mTaskTimes.clear();
    mCornTime.clear();
}