#pragma once
#include "Event.hpp"
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include "nlohmann/json.hpp"

class CustomEvent : public Event {
public:
    std::string    mEventName;
    nlohmann::json mEventData;
    std::string    mFromPlugin;

    KobeBryant_NDAPI CustomEvent(
        const std::string&    eventName,
        const nlohmann::json& eventData,
        const std::string&    fromPlugin = utils::getCurrentPluginName()
    );
};