#include "api/event/MetaEvent.hpp"

MetaEvent::MetaEvent(MetaEventType type, nlohmann::json packet) : Event(packet), mType(type) {}