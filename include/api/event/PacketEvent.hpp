#pragma once
#include "Event.hpp"
#include "Macros.hpp"
#include "nlohmann/json.hpp"

// 基本事件，未分类，不建议监听
class PacketEvent : public Event {
public:
    nlohmann::json mPacket;

    KobeBryant_NDAPI PacketEvent(nlohmann::json packet);

    KobeBryant_NDAPI std::string toString(int format = 4) const;
};