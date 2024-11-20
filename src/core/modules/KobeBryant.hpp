#pragma once
#include "api/ThreadPool.hpp"
#include "api/i18n/LangI18n.hpp"
#include "api/utils/FileUtils.hpp"
#include "api/utils/UUID.hpp"
#include "core/Global.hpp"
#include "lightwebsocketclient/WebSocketClient.h"
#include "resource.hpp"
#include <mutex>

class KobeBryant {
private:
    std::unique_ptr<WebSocketClient>                            mWsClient;
    Logger                                                      mLogger{BOT_NAME};
    std::unique_ptr<i18n::LangI18n>                             mI18n;
    std::string                                                 mUrl;
    std::string                                                 mToken;
    bool                                                        mConnected      = false;
    uint64_t                                                    mNextCallbackId = 0;
    std::map<uint64_t, std::function<void(const std::string&)>> mPacketCallback;
    bool                                                        mColorLog = true;
    std::optional<std::filesystem::path>                        mLogPath;
    std::optional<ThreadPool<>>                                 mThreadPool;

public:
    KobeBryant();

    void init();

    bool hasConnected() const;

    bool shouldColorLog() const;

    std::optional<std::filesystem::path> getLogPath() const;

    static KobeBryant& getInstance();

    std::wstring getProcessMutex() const;

    Logger& getLogger();

    i18n::LangI18n& getI18n();

    WebSocketClient& getWsClient();

    void connect();

    void subscribeLogin(const std::function<void(bool, nlohmann::json)>& callback);

    void sendRawPacket(const std::string& input);

    uint64_t subscribeReceiveRawPacket(const std::function<void(const std::string&)>& callback);

    bool unsubscribeReceiveRawPacket(uint64_t id);

    void printVersion();

    ThreadPool<>& getThreadPool();
};

#define CATCH_END                                                                                                      \
    catch (...) {                                                                                                      \
        KobeBryant::getInstance().getLogger().error("bot.catch.unknownEception");                                      \
    }

#define CATCH                                                                                                          \
    catch (const std::exception& e) {                                                                                  \
        KobeBryant::getInstance().getLogger().error("bot.catch.exception", {e.what()});                                \
    }                                                                                                                  \
    CATCH_END