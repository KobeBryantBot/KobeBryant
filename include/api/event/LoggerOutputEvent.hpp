#pragma once
#include "Event.hpp"
#include "api/Logger.hpp"

class LoggerOutputEvent : public Event {
public:
    std::string      mOutputMessage;
    Logger::LogLevel mLogLevel;
    std::string      mTitle;
    std::string      mTime;

    LoggerOutputEvent(
        std::string const& outputMessage,
        Logger::LogLevel   logLevel,
        std::string const& title,
        std::string const& time
    );
};