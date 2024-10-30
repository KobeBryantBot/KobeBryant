#pragma once
#include "LightWebSocketClient/WebSocketClient.h"
#include "api/i18n/LangI18n.hpp"
#include "api/utils/FileUtils.hpp"
#include "api/utils/UUID.hpp"
#include "core/Global.hpp"
#include "resource.hpp"
#include <mutex>

class KobeBryant {
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

    std::unique_ptr<WebSocketClient>                            mWsClient;
    Logger                                                      mLogger{BOT_NAME};
    std::unique_ptr<i18n::LangI18n>                             mI18n;
    std::string                                                 mUrl;
    std::string                                                 mToken;
    bool                                                        mConnected      = false;
    uint64_t                                                    mNextCallbackId = 0;
    std::map<uint64_t, std::function<void(std::string const&)>> mPacketCallback;
    bool                                                        mColorLog = true;
    std::optional<std::filesystem::path>                        mLogPath;
    std::mutex                                                  mMtx;
    std::condition_variable                                     mCv;
    std::vector<std::shared_ptr<TaskInfo>>                      mTasks;
    std::unordered_map<TaskID, size_t>                          mTaskIndexMap; // Map from task ID to task index
    TaskID                                                      mNextTaskID{0};

public:
    KobeBryant();

    bool hasConnected() const;

    bool shouldColorLog() const;

    std::optional<std::filesystem::path> getLogPath() const;

    static KobeBryant& getInstance();

    std::wstring getProcessMutex() const;

    Logger& getLogger();

    i18n::LangI18n& getI18n();

    WebSocketClient& getWsClient();

    void connect();

    void subscribeLogin(std::function<void(bool, nlohmann::json)> const& callback);

    void sendRawPacket(std::string const& input);

    uint64_t subscribeReceiveRawPacket(std::function<void(std::string const&)> const& callback);

    bool unsubscribeReceiveRawPacket(uint64_t id);

    TaskID addDelayTask(std::chrono::milliseconds delay, Task const& task);

    TaskID addRepeatTask(std::chrono::milliseconds delay, Task const& task);

    bool cancelTask(TaskID id);

    void printVersion();
};

#define CATCH                                                                                                          \
    catch (const std::exception& e) {                                                                                  \
        KobeBryant::getInstance().getLogger().error("bot.catch.exception", {e.what()});                                \
    }
