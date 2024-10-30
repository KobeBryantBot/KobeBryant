#include "api/Logger.hpp"
#include "api/EventBus.hpp"
#include "api/event/LoggerOutputEvent.hpp"
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

std::string getCurrentTimeFormatted() {
    time_t    time_s = time(0);
    struct tm now_tm;
    localtime_s(&now_tm, &time_s);
    std::stringstream ss;
    ss << std::put_time(&now_tm, "[%Y-%m-%d %H:%M:%S]");
    return ss.str();
}

std::string getLoggerPrefix(Logger::LogLevel level) {
    static std::vector<std::string> map = {"[FATAL]", "[ERROR]", "[WARN]", "[INFO]", "[DEBUG]"};
    return map[(int)level];
}

Logger::Logger() = default;

Logger::Logger(std::string const& title) { mTitle = "[" + title + "]"; }

void Logger::setTitle(std::string const& title) { mTitle = "[" + title + "]"; }

void Logger::setLevel(LogLevel level) { mLogLevel = level; }

bool Logger::setFile(std::filesystem::path const& path) {
    if (!mFilePath) {
        mFilePath = path;
        if (!std::filesystem::exists(path.parent_path())) {
            std::filesystem::create_directories(path.parent_path());
        }
        return true;
    }
    return false;
}

void logToFile(std::filesystem::path const& path, std::string const& logStr) {
    std::ofstream fileStream(path, std::ios::app);
    fileStream << logStr << std::endl;
    fileStream.flush();
    fileStream.close();
}

void Logger::printStr(LogLevel level, std::string const& data) const {
    try {
        if (mLogLevel >= level) {
            auto              timeStr = getCurrentTimeFormatted();
            auto              prefix  = getLoggerPrefix(level);
            auto              logStr  = fmt::format("{} {} {} {}", timeStr, prefix, mTitle, data);
            LoggerOutputEvent ev(data, level, mTitle, timeStr);
            EventBus::getInstance().publish(ev);
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
                return fmt::print("{} {} {} {}\n", time, COLOR(fmt::color::light_sea_green, prefix), mTitle, data);
            }
            }
        }
    }
    CATCH
}

std::string Logger::translate(std::string const& data, std::vector<std::string> const& params) const {
    return tr(data, params);
}