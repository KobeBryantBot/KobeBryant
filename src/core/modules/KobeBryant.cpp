#include "KobeBryant.hpp"
#include "ScheduleManager.hpp"
#include "api/utils/ModuleUtils.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/Version.hpp"
#include "fmt/format.h"

namespace fs = std::filesystem;

std::string tr(const std::string& key, const std::vector<std::string>& params) {
    return KobeBryant::getInstance().getI18n().translate(key, params);
}

nlohmann::ordered_json loadConfig() {
    auto rawConfig = utils::readResource(NULL, DEFAULT_CONFIG);
    auto config    = nlohmann::ordered_json::parse(*rawConfig, nullptr, true, true);
    if (auto oldConfig = utils::readFile("./config/config.json")) {
        try {
            config.merge_patch(nlohmann::ordered_json::parse(*oldConfig, nullptr, true, true));
        }
        CATCH
    }
    utils::writeFile("./config/config.json", config.dump(4));
    return config;
}

KobeBryant::KobeBryant() {
    try {
        auto config = loadConfig();
        // 初始化日志系统
        int logLevel = config["logger"]["level"];
        mLogger.setLevel(Logger::LogLevel(logLevel));
        if (bool logFile = config["logger"]["logFile"]) {
            mLogPath = (std::string)config["logger"]["logFilePath"];
            if (!mLogPath->empty()) {
                auto path = std::filesystem::path(*mLogPath);
                if (!std::filesystem::exists(path.parent_path())) {
                    std::filesystem::create_directories(path.parent_path());
                }
            }
        }
        mColorLog = config["logger"]["colorLog"];
        // 初始化语言系统
        std::string lang = config["language"];
        mI18n            = std::make_unique<i18n::LangI18n>("./lang", lang);
        mI18n->updateOrCreateLanguage("zh_CN", *utils::readResource(NULL, DEFAULT_ZH_CN));
        mI18n->loadAllLanguages();
        // 创建客户端
        mUrl      = config["ws_url"];
        mToken    = config["token"];
        mWsClient = std::make_unique<WebSocketClient>();
        // 初始化线程池
        mThreadPool.emplace(2);
    }
    CATCH
}

void KobeBryant::init() {
    try { // 处理数据包
        getWsClient().OnTextReceived([&](WebSocketClient& client, std::string text) {
            for (auto& [id, callback] : mPacketCallback) {
                if (callback) {
                    try {
                        callback(text);
                    } catch (const std::exception& ex) {
                        getLogger().error("bot.error.callback", {ex.what()});
                    }
                    CATCH_END
                }
            }
        });
        // 重连
        getWsClient().OnLostConnection([&](WebSocketClient& client, int code) {
            try {
                if (mConnected) {
                    getLogger().error("bot.error.lostConnection", {S(code)});
                }
                mConnected = false;
                Scheduler::getInstance().addDelayTask(std::chrono::milliseconds(10000), [&] {
                    getLogger().info("bot.main.reconnecting");
                    connect();
                });
            } catch (const std::exception& ex) {
                getLogger().error("bot.error.reconnecting", {ex.what()});
            }
            CATCH_END
        });
        // 登录
        subscribeLogin([&](bool success, nlohmann::json data) {
            if (success) {
                mConnected = true;
                int64_t qq = data["self_id"];
                getLogger().info("bot.main.connected", {S(qq)});
            } else {
                std::string reason = data["message"];
                getLogger().error("bot.error.connect", {reason});
            }
        });
    }
    CATCH
}

void KobeBryant::subscribeLogin(const std::function<void(bool, nlohmann::json)>& callback) {
    subscribeReceiveRawPacket([=](const std::string& text) {
        try {
            if (callback) {
                auto json = nlohmann::json::parse(text);
                if (json.contains("retcode") && json["retcode"] == 1403 && json.contains("status")
                    && json["status"] == "failed") {
                    callback(false, json);
                } else if (json.contains("sub_type") && json["sub_type"] == "connect"
                           && json.contains("meta_event_type") && json["meta_event_type"] == "lifecycle") {
                    callback(true, json);
                }
            }
        }
        CATCH
    });
}

KobeBryant& KobeBryant::getInstance() {
    static std::unique_ptr<KobeBryant> instance;
    if (!instance) {
        instance = std::make_unique<KobeBryant>();
    }
    return *instance;
}

void KobeBryant::connect() {
    getThreadPool().enqueue([&] {
        try {
            getWsClient().Connect(mUrl, mToken);
        } catch (const std::exception& ex) {
            getLogger().error("bot.error.connect", {ex.what()});
            Scheduler::getInstance().addDelayTask(std::chrono::milliseconds(10000), [&] {
                getLogger().info("bot.main.reconnecting");
                connect();
            });
        }
        CATCH_END
    });
}

Logger& KobeBryant::getLogger() { return mLogger; }

i18n::LangI18n& KobeBryant::getI18n() { return *mI18n; }

WebSocketClient& KobeBryant::getWsClient() { return *mWsClient; }

void KobeBryant::sendRawPacket(const std::string& input) {
    try {
        getWsClient().SendText(input);
    } catch (const std::exception& ex) {
        getLogger().error("bot.error.send", {ex.what()});
    }
    CATCH_END
}

bool KobeBryant::hasConnected() const { return mConnected; }

uint64_t KobeBryant::subscribeReceiveRawPacket(const std::function<void(const std::string&)>& callback) {
    mNextCallbackId++;
    mPacketCallback[mNextCallbackId] = std::move(callback);
    return mNextCallbackId;
}

bool KobeBryant::unsubscribeReceiveRawPacket(uint64_t id) {
    if (mPacketCallback.contains(id)) {
        mPacketCallback.erase(id);
        return true;
    }
    return false;
}

bool KobeBryant::shouldColorLog() const { return mColorLog; }

std::optional<std::filesystem::path> KobeBryant::getLogPath() const { return mLogPath; }

std::wstring KobeBryant::getProcessMutex() const {
    if (auto key = utils::readFile("./process.key", true)) {
        if (key->size() == 16) {
            auto uuid = utils::UUID::fromBinary(*key);
            return utils::stringtoWstring(uuid.toString());
        }
    }
    auto uuid = utils::UUID::random();
    utils::writeFile("./process.key", uuid.toBinary(), true);
    return utils::stringtoWstring(uuid.toString());
}

void KobeBryant::printVersion() {
    getLogger().info("{}: {}", tr("bot.main.start"), KOBE_VERSION_COLOR_STRING);
    getLogger().info("Copyright © 2024 KobeBryantBot. All rights reserved.");
}

ThreadPool<>& KobeBryant::getThreadPool() { return *mThreadPool; }