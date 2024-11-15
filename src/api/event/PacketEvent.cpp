#include "api/event/PacketEvent.hpp"

PacketEvent::PacketEvent(nlohmann::json packet) : mPacket(packet) {}

std::string PacketEvent::toString(int format) { return mPacket.dump(format); }