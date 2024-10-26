#pragma once
#include "PacketEvent.hpp"
#include "api/Enums.hpp"
#include <optional>

class NoticeEvent : public PacketEvent {
public:
    NoticeType                   mType;
    std::optional<NotifySubType> mSubType;
    std::optional<uint64_t>      mGroup;
    std::optional<uint64_t>      mSender;
    std::optional<uint64_t>      mSelf;

    NoticeEvent(
        NoticeType                   type,
        std::optional<NotifySubType> subType,
        std::optional<uint64_t>      group,
        std::optional<uint64_t>      sender,
        std::optional<uint64_t>      self,
        nlohmann::json               packet
    );
};
