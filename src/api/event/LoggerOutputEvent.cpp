#include "api/event/LoggerOutputEvent.hpp"

LoggerOutputEvent::LoggerOutputEvent(
    std::string const& outputMessage,
    Logger::LogLevel   logLevel,
    std::string const& title,
    std::string const& time
)
: mOutputMessage(outputMessage),
  mLogLevel(logLevel),
  mTitle(title),
  mTime(time) {}