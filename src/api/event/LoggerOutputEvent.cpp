#include "api/event/LoggerOutputEvent.hpp"

LoggerOutputEvent::LoggerOutputEvent(
    std::string&      outputMessage,
    Logger::LogLevel& logLevel,
    std::string&      title,
    std::string&      time
)
: mOutputMessage(outputMessage),
  mLogLevel(logLevel),
  mTitle(title),
  mTime(time) {}