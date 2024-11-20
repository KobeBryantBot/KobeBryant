#include "api/event/CustomEvent.hpp"

CustomEvent::CustomEvent(const std::string& eventName, const nlohmann::json& eventData, const std::string& fromPlugin)
: mEventName(eventName),
  mEventData(eventData),
  mFromPlugin(fromPlugin) {}