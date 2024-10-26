#include "api/event/NoticeEvent.hpp"

NoticeEvent::NoticeEvent(
    NoticeType                   type,
    std::optional<NotifySubType> subType,
    std::optional<uint64_t>      group,
    std::optional<uint64_t>      sender,
    std::optional<uint64_t>      self,
    nlohmann::json               packet
)
: PacketEvent(packet),
  mType(type),
  mSubType(subType),
  mGroup(group),
  mSelf(self),
  mSender(sender) {}