#pragma once
#include "Event.hpp"
#include "api/Enums.hpp"

class MetaEvent : public Event {
public:
    MetaEventType mType;

    MetaEvent(MetaEventType type, nlohmann::json packet);
};