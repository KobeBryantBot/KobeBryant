#pragma once
#include "PacketEvent.hpp"
#include "api/Enums.hpp"

class MetaEvent : public PacketEvent {
public:
    MetaEventType mType;

    MetaEvent(MetaEventType type, nlohmann::json packet);
};