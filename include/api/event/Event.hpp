#pragma once
#include "Macros.hpp"
#include "nlohmann/json.hpp"

// 基本事件，未分类，不建议监听
class Event {
public:
    nlohmann::json mPacket;

    virtual ~Event() = default;

    KobeBryant_NDAPI Event(nlohmann::json packet);

    KobeBryant_NDAPI std::string toString(int format = 4) const;
};