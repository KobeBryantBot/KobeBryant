#pragma once
#include "PacketEvent.hpp"
#include "api/Enums.hpp"
#include <optional>

class MessageEvent : public PacketEvent {
public:
    MessageType             mType;
    MessageSubType          mSubType;
    uint64_t                mSelf;
    std::optional<uint64_t> mGroup;
    uint64_t                mSender;
    std::string             mRawMessage;
    int64_t                 mMessageId;

    MessageEvent(
        MessageType             type,
        MessageSubType          subType,
        uint64_t                self,
        std::optional<uint64_t> group,
        uint64_t                sender,
        std::string             rawMessage,
        int64_t                 messageId,
        nlohmann::json          packet
    );
};