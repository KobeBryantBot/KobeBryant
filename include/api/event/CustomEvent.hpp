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
        std::string const&    eventName,
        nlohmann::json const& eventData,
        std::string const&    fromPlugin = utils::getCurrentPluginName()
    );
};