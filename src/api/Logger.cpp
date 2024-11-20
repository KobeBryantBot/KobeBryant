#include "api/Logger.hpp"
#include "api/EventBus.hpp"
#include "api/event/LoggerOutputEvent.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/Global.hpp"
#include "core/modules/KobeBryant.hpp"
#include "fmt/base.h"
#include "fmt/color.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#define COLOR(color, data) fmt::format(fg(color), fmt::runtime(data))

std::string getLoggerPrefix(Logger::LogLevel level) {
    static std::vector<std::string> map = {"[TRACE]", "[FATAL]", "[ERROR]", "[WARN]", "[INFO]", "[DEBUG]"};
    return map[(int)level];
}

Logger::Logger(const std::string& title) { mTitle = "[" + title + "]"; }

void Logger::setTitle(const std::string& title) { mTitle = "[" + title + "]"; }

void Logger::setLevel(LogLevel level) { mLogLevel = level; }

bool Logger::setFile(const std::filesystem::path& path) {
    if (!mFilePath) {
        mFilePath = path;
        if (!std::filesystem::exists(path.parent_path())) {
            std::filesystem::create_directories(path.parent_path());
        }
        return true;
    }
    return false;
}

void logToFile(const std::filesystem::path& path, const std::string& logStr) {
    KobeBryant::getInstance().getThreadPool().enqueue([=] {
        std::ofstream fileStream(path, std::ios::app);
        fileStream << logStr << std::endl;
        fileStream.flush();
        fileStream.close();
    });
}

std::string getLastDate() {
    static std::optional<std::string> mLastDate;
    if (!mLastDate) {
        mLastDate = utils::getTimeStringFormatted("%Y-%m-%d");
    }
    return *mLastDate;
}

void backupLog() {
    if (std::filesystem::exists("./logs/latest.log")) {
        if (!std::filesystem::exists("./logs/backup")) {
            std::filesystem::create_directories("./logs/backup");
        }
        auto logs     = utils::getAllFileFullNameInDirectory("./logs/backup");
        int  count    = 1;
        auto lastDate = getLastDate();
        for (auto& log : logs) {
            if (log.starts_with(lastDate)) {
                count++;
            }
        }
        std::string newPath = fmt::format("./logs/backup/{}-{}.log", lastDate, count);
        if (std::filesystem::exists(newPath)) {
            std::filesystem::remove(newPath);
        }
        std::filesystem::copy("./logs/latest.log", newPath);
        std::filesystem::remove("./logs/latest.log");
    }
}

void Logger::printStr(LogLevel level, std::string&& data) const noexcept {
    if (getLastDate() != utils::getTimeStringFormatted("%Y-%m-%d")) {
        backupLog();
    }
    if (mLogLevel >= level) {
        auto              timeStr = utils::getTimeStringFormatted("[%Y-%m-%d %H:%M:%S]");
        auto              prefix  = getLoggerPrefix(level);
        auto              title   = mTitle;
        LoggerOutputEvent ev(data, level, title, timeStr);
        EventBus::getInstance().publish(ev);
        auto logStr = fmt::format("{} {} {} {}", timeStr, prefix, title, data);
        if (!ev.isCancelled()) {
            if (auto globalPath = KobeBryant::getInstance().getLogPath()) {
                logToFile(*globalPath, logStr);
            }
            if (mFilePath) {
                logToFile(*mFilePath, logStr);
            }
            if (!KobeBryant::getInstance().shouldColorLog()) {
                return fmt::print("{} {} {} {}\n", timeStr, prefix, mTitle, data);
            }
            auto time = COLOR(fmt::color::light_blue, timeStr);
            switch (level) {
            case LogLevel::Trace: {
                return fmt::print("{} {} {} {}\n", timeStr, prefix, mTitle, data);
            }
            case LogLevel::Fatal: {
                return fmt::print("{} \x1b[31m{} {} {}\x1b[0m\n", time, prefix, mTitle, data);
            }
            case LogLevel::Error: {
                return fmt::print("{} \x1b[91m{} {} {}\x1b[0m\n", time, prefix, mTitle, data);
            }
            case LogLevel::Warn: {
                return fmt::print("{} \x1b[93m{} {} {}\x1b[0m\n", time, prefix, mTitle, data);
            }
            case LogLevel::Debug: {
                return fmt::print("{} \x1b[90m{} {} {}\x1b[0m\n", time, prefix, mTitle, data);
            }
            default: {
                return fmt::print("{} \x1b[36m{} \x1b[0m{} {}\n", time, prefix, mTitle, data);
            }
            }
        }
    }
}

void Logger::printView(LogLevel level, std::string_view data) const noexcept { printStr(level, std::string(data)); }

std::string Logger::translate(std::string_view data, const std::vector<std::string>& params) const {
    return tr(std::string(data), params);
}

void Logger::appendLanguage(const std::string& local, const i18n::LangFile& lang) {
    KobeBryant::getInstance().getI18n().appendLanguage(local, lang);
}

void Logger::appendLanguage(const std::string& local, const std::string& lang) {
    KobeBryant::getInstance().getI18n().appendLanguage(local, lang);
}