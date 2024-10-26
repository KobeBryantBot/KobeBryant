#include "api/event/NoticeEvent.hpp"

NoticeEvent::NoticeEvent(
    NoticeType                   type,
    std::optional<NotifySubType> subType,
    std::optional<uint64_t>      group,
    std::optional<uint64_t>      sender,
    nlohmann::json               packet
)
: PacketEvent(packet),
  mType(type),
  mSubType(subType),
  mGroup(group),
  mSender(sender) {}