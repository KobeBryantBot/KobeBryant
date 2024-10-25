#include "api/event/Event.hpp"

Event::Event(nlohmann::json packet) : mPacket(packet) {}

std::string Event::toString(int format) const { return mPacket.dump(format); }