#include "KobeBryant.hpp"
#include "api/utils/ModuleUtils.hpp"
#include "api/utils/StringUtils.hpp"
#include "fmt/color.h"
#include "fmt/format.h"

namespace fs = std::filesystem;

std::string tr(std::string const& key, std::vector<std::string> const& params) {
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
        int logLevel = config["logger"]["logLevel"];
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
        // 处理数据包
        getWsClient().OnTextReceived([&](WebSocketClient& client, std::string text) {
            for (auto& [id, callback] : mPacketCallback) {
                if (callback) {
                    try {
                        callback(text);
                    } catch (const std::exception& ex) {
                        getLogger().error("bot.error.callback", {ex.what()});
                    }
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
                addDelayTask(10, [&] {
                    getLogger().info("bot.main.reconnecting");
                    connect();
                });
            } catch (const std::exception& ex) {
                getLogger().error("bot.error.reconnecting", {ex.what()});
            }
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
        std::thread([&] {
            while (EXIST_FLAG) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                for (auto& [id, delay] : mTaskDelay) {
                    delay--;
                    if (delay <= 0) {
                        if (auto& func = mTasks[id]) {
                            func();
                            if (mTaskRepeat.contains(id)) {
                                delay = mTaskRepeat[id];
                            } else {
                                mTaskDelay.erase(id);
                                mTasks.erase(id);
                            }
                        } else {
                            mTasks.erase(id);
                        }
                    }
                }
            }
        }).detach();
    }
    CATCH
}

void KobeBryant::subscribeLogin(std::function<void(bool, nlohmann::json)> const& callback) {
    subscribeReceiveRawPacket([=](std::string const& text) {
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
    try {
        getWsClient().Connect(mUrl, mToken);
    } catch (const std::exception& ex) {
        getLogger().error("bot.error.connect", {ex.what()});
        addDelayTask(10, [&] {
            getLogger().info("bot.main.reconnecting");
            connect();
        });
    }
}

Logger& KobeBryant::getLogger() { return mLogger; }

i18n::LangI18n& KobeBryant::getI18n() { return *mI18n; }

WebSocketClient& KobeBryant::getWsClient() { return *mWsClient; }

void KobeBryant::sendRawPacket(std::string const& input) {
    try {
        getWsClient().SendText(input);
    } catch (const std::exception& ex) {
        getLogger().error("bot.error.send", {ex.what()});
    }
}

bool KobeBryant::hasConnected() const { return mConnected; }

uint64_t KobeBryant::subscribeReceiveRawPacket(std::function<void(std::string const&)> const& callback) {
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

uint64_t KobeBryant::addDelayTask(uint64_t seconds, std::function<void()> const& task) {
    mNextTaskId++;
    auto id        = mNextTaskId;
    mTasks[id]     = std::move(task);
    mTaskDelay[id] = seconds;
    return id;
}

uint64_t KobeBryant::addRepeatTask(uint64_t seconds, std::function<void()> const& task) {
    mNextTaskId++;
    auto id         = mNextTaskId;
    mTasks[id]      = std::move(task);
    mTaskRepeat[id] = seconds;
    mTaskDelay[id]  = seconds;
    return id;
}

bool KobeBryant::cancelTask(uint64_t id) {
    std::lock_guard lock{mMutex};
    if (mTaskDelay.contains(id)) {
        mTasks.erase(id);
        mTaskRepeat.erase(id);
        mTaskDelay.erase(id);
        return true;
    }
    return false;
}

std::wstring KobeBryant::getProcessMutex() const {
    if (auto key = utils::readFile("./process.key")) {
        if (key->size() == 36) {
            auto result = *key;
            return utils::toWstring(result);
        }
    }
    auto result = utils::UUID::random().toString();
    utils::writeFile("./process.key", result);
    return utils::toWstring(result);
}

void KobeBryant::printVersion() {
    getLogger().info(
        "{}: {}",
        tr("bot.main.start"),
        fmt::format(
            fg(fmt::color::pink),
            fmt::runtime(
                fmt::format("{}-{}.{}.{}", BOT_NAME, KOBE_VERSION_MAJOR, KOBE_VERSION_MINOR, KOBE_VERSION_PATCH)
            )
        )
    );
    getLogger().info("Copyright (C) 2024 KobeBryantBot. All rights reserved.");
}