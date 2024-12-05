#include "api/event/RequestEvent.hpp"

RequestEvent::RequestEvent(
    RequestType                   type,
    std::optional<RequestSubType> subType,
    uint64_t                      sender,
    uint64_t                      self,
    std::string                   comment,
    std::string                   flag,
    std::optional<uint64_t>       group,
    nlohmann::json                packet
)
: PacketEvent(packet),
  mType(type),
  mSubType(subType),
  mSender(sender),
  mSelf(self),
  mComment(comment),
  mFlag(flag),
  mGroup(group) {}