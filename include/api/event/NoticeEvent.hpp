#pragma once
#include "PacketEvent.hpp"
#include "api/Enums.hpp"
#include <optional>

class NoticeEvent : public PacketEvent {
public:
    NoticeType                   mType;
    std::optional<NotifySubType> mSubType;

    NoticeEvent(NoticeType type, std::optional<NotifySubType> subType, nlohmann::json packet);
};
