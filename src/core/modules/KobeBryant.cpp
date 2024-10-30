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
                addDelayTask(std::chrono::milliseconds(10000), [&] {
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
                std::unique_lock<std::mutex> lock(mMtx);
                mCv.wait(lock, [this] { return !mTasks.empty(); });

                auto              now = std::chrono::high_resolution_clock::now();
                std::vector<Task> readyTasks;

                // Collect all tasks that are ready to run
                for (size_t i = 0; i < mTasks.size(); ++i) {
                    auto& taskPtr = mTasks[i];
                    if (taskPtr->cancelled.load()) {
                        mTaskIndexMap.erase(taskPtr->id);
                        continue;
                    }

                    if (taskPtr->runTime <= now) {
                        readyTasks.push_back(taskPtr->task);
                        if (taskPtr->interval.count() > 0) {
                            taskPtr->runTime = now + taskPtr->interval;
                        } else {
                            mTaskIndexMap.erase(taskPtr->id);
                            mTasks.erase(mTasks.begin() + i);
                            --i; // Adjust index after erase
                        }
                    }
                }

                // Notify the main thread to execute the collected tasks
                lock.unlock();
                for (auto& task : readyTasks) {
                    if (task) {
                        task(); // Execute the task in the main thread
                    }
                }
            }
            mCv.notify_all();
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
        addDelayTask(std::chrono::milliseconds(10000), [&] {
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

KobeBryant::TaskID KobeBryant::addDelayTask(std::chrono::milliseconds delay, Task const& task) {
    std::lock_guard<std::mutex> lock(mMtx);
    TaskID                      id       = mNextTaskID++;
    auto                        taskInfo = std::make_shared<TaskInfo>(
        id,
        std::move(task),
        std::chrono::steady_clock::now() + delay,
        std::chrono::milliseconds(0)
    );
    mTasks.push_back(taskInfo);
    mTaskIndexMap[id] = mTasks.size() - 1;
    mCv.notify_one();
    return id;
}

KobeBryant::TaskID KobeBryant::addRepeatTask(std::chrono::milliseconds interval, Task const& task) {
    std::lock_guard<std::mutex> lock(mMtx);
    TaskID                      id = mNextTaskID++;
    auto                        taskInfo =
        std::make_shared<TaskInfo>(id, std::move(task), std::chrono::steady_clock::now() + interval, interval);
    mTasks.push_back(taskInfo);
    mTaskIndexMap[id] = mTasks.size() - 1;
    mCv.notify_one();
    return id;
}

bool KobeBryant::cancelTask(TaskID id) {
    std::lock_guard<std::mutex> lock(mMtx);
    auto                        it = mTaskIndexMap.find(id);
    if (it != mTaskIndexMap.end()) {
        mTasks[it->second]->cancelled.store(true);
        return true;
    }
    return false;
}

std::wstring KobeBryant::getProcessMutex() const {
    if (auto key = utils::readFile("./process.key", true)) {
        if (key->size() == 16) {
            auto uuid = utils::UUID::fromBinary(*key);
            return utils::toWstring(uuid.toString());
        }
    }
    auto uuid = utils::UUID::random();
    utils::writeFile("./process.key", uuid.toBinary(), true);
    return utils::toWstring(uuid.toString());
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
    getLogger().info("Copyright © 2024 KobeBryantBot. All rights reserved.");
}