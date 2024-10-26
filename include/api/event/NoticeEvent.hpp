#pragma once
#include "PacketEvent.hpp"
#include "api/Enums.hpp"
#include <optional>

class NoticeEvent : public PacketEvent {
public:
    NoticeType                   mType;
    std::optional<NotifySubType> mSubType;
    std::optional<uint64_t>      mGroup;

    NoticeEvent(
        NoticeType                   type,
        std::optional<NotifySubType> subType,
        std::optional<uint64_t>      group,
        nlohmann::json               packet
    );
};
