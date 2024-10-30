#include "api/event/MetaEvent.hpp"

MetaEvent::MetaEvent(MetaEventType type, nlohmann::json packet) : PacketEvent(packet), mType(type) {}