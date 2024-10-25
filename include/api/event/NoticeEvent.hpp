#pragma once
#include "Event.hpp"
#include "api/Enums.hpp"
#include <optional>

class NoticeEvent : public Event {
public:
    NoticeType                   mType;
    std::optional<NotifySubType> mSubType;

    NoticeEvent(NoticeType type, std::optional<NotifySubType> subType, nlohmann::json packet);
};
