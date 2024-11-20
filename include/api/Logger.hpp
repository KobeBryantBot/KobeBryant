#pragma once
#include "Macros.hpp"
#include "api/i18n/base/LangFile.hpp"
#include "api/utils/ModuleUtils.hpp"
#include "fmt/format.h"
#include <filesystem>
#include <optional>

class Logger {
public:
    enum class LogLevel : int {
        Trace = 0,
        Fatal = 1,
        Error = 2,
        Warn  = 3,
        Info  = 4, // Default
        Debug = 5,
    };

protected:
    std::string                          mTitle;
    LogLevel                             mLogLevel = LogLevel::Info;
    std::optional<std::filesystem::path> mFilePath = {};

public:
    KobeBryant_API static void appendLanguage(const std::string& local, const i18n::LangFile& lang);

    KobeBryant_API static void appendLanguage(const std::string& local, const std::string& lang);

    KobeBryant_NDAPI explicit Logger(const std::string& title = utils::getCurrentPluginName());

    KobeBryant_API void setTitle(const std::string& title);

    KobeBryant_API void setLevel(LogLevel level);

    KobeBryant_API bool setFile(const std::filesystem::path& path);

    KobeBryant_API void printStr(LogLevel level, std::string&& data) const noexcept;

    KobeBryant_API void printView(LogLevel level, std::string_view data) const noexcept;

    KobeBryant_NDAPI std::string translate(std::string_view data, const std::vector<std::string>& params) const;

public:
    template <typename... Args>
    void log(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(level, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    void log(LogLevel level, std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(level, translate(msg, params));
    }

    template <typename... Args>
    void fatal(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Fatal, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    void fatal(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Fatal, translate(msg, params));
    }

    template <typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Error, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    void error(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Error, translate(msg, params));
    }

    template <typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Warn, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    void warn(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Warn, translate(msg, params));
    }

    template <typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Info, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    void info(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Info, translate(msg, params));
    }

    template <typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Debug, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    void debug(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Debug, translate(msg, params));
    }
};