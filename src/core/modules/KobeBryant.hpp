#pragma once
#include "LightWebSocketClient/WebSocketClient.h"
#include "api/i18n/LangI18n.hpp"
#include "api/utils/FileUtils.hpp"
#include "core/Global.hpp"
#include "resource.hpp"
#include <mutex>

class KobeBryant {
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
    uint64_t                                                    mNextTaskId = 0;
    std::unordered_map<uint64_t, std::function<void()>>         mTasks;
    std::unordered_map<uint64_t, int64_t>                       mTaskDelay;
    std::unordered_map<uint64_t, int64_t>                       mTaskRepeat;
    std::mutex                                                  mMutex;

public:
    KobeBryant();

    bool hasConnected() const;

    bool shouldColorLog() const;

    std::optional<std::filesystem::path> getLogPath() const;

    static KobeBryant& getInstance();

    Logger& getLogger();

    i18n::LangI18n& getI18n();

    WebSocketClient& getWsClient();

    void connect();

    void subscribeLogin(std::function<void(bool, nlohmann::json)> const& callback);

    void sendRawPacket(std::string const& input);

    uint64_t subscribeReceiveRawPacket(std::function<void(std::string const&)> const& callback);

    bool unsubscribeReceiveRawPacket(uint64_t id);

    uint64_t addDelayTask(uint64_t seconds, std::function<void()> const& task);

    uint64_t addRepeatTask(uint64_t seconds, std::function<void()> const& task);

    bool cancelTask(uint64_t id);
};
