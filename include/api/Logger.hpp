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
    /**
     * 向日志中添加语言文件
     * @param local 语言文件的本地路径
     * @param lang 语言文件的内容
     */
    KobeBryant_API static void appendLanguage(const std::string& local, const i18n::LangFile& lang);

    /**
     * 向日志中添加语言文件
     * @param local 语言文件的本地路径
     * @param lang 语言文件的内容
     */
    KobeBryant_API static void appendLanguage(const std::string& local, const std::string& lang);

    /**
     * 构造函数
     * @param title 日志标题
     */
    KobeBryant_NDAPI explicit Logger(const std::string& title = utils::getCurrentPluginName());

    /**
     * 设置日志标题
     * @param title 日志标题
     */
    KobeBryant_API void setTitle(const std::string& title);

    /**
     * 设置日志级别
     * @param level 日志级别
     */
    KobeBryant_API void setLevel(LogLevel level);

    /**
     * 设置日志文件路径
     * @param path 日志文件路径
     * @return 如果设置成功返回 true，否则返回 false
     */
    KobeBryant_API bool setFile(const std::filesystem::path& path);

    /**
     * 打印日志
     * @param level 日志级别
     * @param data 日志数据
     */
    KobeBryant_API void printStr(LogLevel level, std::string&& data) const noexcept;

    /**
     * 打印日志
     * @param level 日志级别
     * @param data 日志数据
     */
    KobeBryant_API void printView(LogLevel level, std::string_view data) const noexcept;

    /**
     * 翻译日志消息
     * @param data 日志消息
     * @param params 日志消息的参数
     * @return 翻译后的日志消息
     */
    KobeBryant_NDAPI std::string translate(std::string_view data, const std::vector<std::string>& params) const;

public:
    /**
     * 打印日志
     * @param level 日志级别
     * @param fmt 日志消息的格式字符串
     * @param args 日志消息的参数
     */
    template <typename... Args>
    void log(LogLevel level, fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(level, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    /**
     * 打印日志
     * @param level 日志级别
     * @param msg 日志消息
     * @param params 日志消息的参数
     */
    void log(LogLevel level, std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(level, translate(msg, params));
    }

    /**
     * 打印日志
     * @param fmt 日志消息的格式字符串
     * @param args 日志消息的参数
     */
    template <typename... Args>
    void fatal(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Fatal, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    /**
     * 打印日志
     * @param msg 日志消息
     * @param params 日志消息的参数
     */
    void fatal(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Fatal, translate(msg, params));
    }

    /**
     * 打印日志
     * @param fmt 日志消息的格式字符串
     * @param args 日志消息的参数
     */
    template <typename... Args>
    void error(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Error, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    /**
     * 打印日志
     * @param msg 日志消息
     * @param params 日志消息的参数
     */
    void error(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Error, translate(msg, params));
    }

    /**
     * 打印日志
     * @param fmt 日志消息的格式字符串
     * @param args 日志消息的参数
     */
    template <typename... Args>
    void warn(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Warn, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    /**
     * 打印日志
     * @param msg 日志消息
     * @param params 日志消息的参数
     */
    void warn(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Warn, translate(msg, params));
    }

    /**
     * 打印日志
     * @param fmt 日志消息的格式字符串
     * @param args 日志消息的参数
     */
    template <typename... Args>
    void info(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Info, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    /**
     * 打印日志
     * @param msg 日志消息
     * @param params 日志消息的参数
     */
    void info(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Info, translate(msg, params));
    }

    /**
     * 打印日志
     * @param fmt 日志消息的格式字符串
     * @param args 日志消息的参数
     */
    template <typename... Args>
    void debug(fmt::format_string<Args...> fmt, Args&&... args) const {
        printStr(LogLevel::Debug, fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }
    /**
     * 打印日志
     * @param msg 日志消息
     * @param params 日志消息的参数
     */
    void debug(std::string_view msg, const std::vector<std::string>& params = {}) const {
        printView(LogLevel::Debug, translate(msg, params));
    }
};