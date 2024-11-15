#pragma once
#include "Event.hpp"
#include "api/Logger.hpp"

class LoggerOutputEvent : public CancellableEvent {
public:
    std::string&      mOutputMessage;
    Logger::LogLevel& mLogLevel;
    std::string&      mTitle;
    std::string&      mTime;

    LoggerOutputEvent(std::string& outputMessage, Logger::LogLevel& logLevel, std::string& title, std::string& time);
};