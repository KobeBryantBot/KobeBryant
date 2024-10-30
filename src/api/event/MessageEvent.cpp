#include "api/event/MessageEvent.hpp"

MessageEvent::MessageEvent(
    MessageType             type,
    MessageSubType          subType,
    uint64_t                self,
    std::optional<uint64_t> group,
    uint64_t                sender,
    std::string             rawMessage,
    int64_t                 messageId,
    nlohmann::json          packet
)
: PacketEvent(packet),
  mType(type),
  mSubType(subType),
  mSelf(self),
  mGroup(group),
  mSender(sender),
  mRawMessage(rawMessage),
  mMessageId(messageId) {}