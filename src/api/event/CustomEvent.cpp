#include "api/event/CustomEvent.hpp"

CustomEvent::CustomEvent(std::string const& eventName, nlohmann::json const& eventData, std::string const& fromPlugin)
: mEventName(eventName),
  mEventData(eventData),
  mFromPlugin(fromPlugin) {}