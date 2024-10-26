#pragma once
#include "PacketEvent.hpp"
#include "api/Enums.hpp"
#include <optional>

class RequestEvent : public PacketEvent {
public:
    RequestType                   mType;
    std::optional<RequestSubType> mSubType;
    uint64_t                      mSender;
    uint64_t                      mSelf;
    std::string                   mComment;
    std::string                   mFlag;
    std::optional<uint64_t>       mGroup;


    RequestEvent(
        RequestType                   type,
        std::optional<RequestSubType> subType,
        uint64_t                      sender,
        uint64_t                      self,
        std::string                   comment,
        std::string                   flag,
        std::optional<uint64_t>       group,
        nlohmann::json                packet
    );
};
