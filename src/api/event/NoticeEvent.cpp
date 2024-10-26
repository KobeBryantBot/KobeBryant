#include "api/event/NoticeEvent.hpp"

NoticeEvent::NoticeEvent(NoticeType type, std::optional<NotifySubType> subType, nlohmann::json packet)
: PacketEvent(packet),
  mType(type),
  mSubType(subType) {}