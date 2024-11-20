#include "EventDispatcher.hpp"
#include "KobeBryant.hpp"
#include "ScheduleManager.hpp"
#include "api/EventBus.hpp"
#include "api/event/MessageEvent.hpp"
#include "api/event/MetaEvent.hpp"
#include "api/event/NoticeEvent.hpp"
#include "api/event/RequestEvent.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/Global.hpp"
#include "magic_enum/magic_enum.hpp"

#define ENUM_CAST(e, x) *magic_enum::enum_cast<e>(utils::toCamelCase((std::string)packet[x]))

EventDispatcher& EventDispatcher::getInstance() {
    static std::unique_ptr<EventDispatcher> instance;
    if (!instance) {
        instance = std::make_unique<EventDispatcher>();
    }
    return *instance;
}

void EventDispatcher::init() {
    KobeBryant::getInstance().subscribeReceiveRawPacket([&](const std::string& rawText) {
        try {
            if (KobeBryant::getInstance().hasConnected()) {
                auto& eventBus = EventBus::getInstance();
                auto  packet   = nlohmann::json::parse(rawText);
                auto  event    = PacketEvent(packet);
                eventBus.publish(event);
                // 解析事件
                if (packet.contains("post_type")) {
                    std::string type = packet["post_type"];
                    switch (utils::doHash(type)) {
                    case utils::doHash("meta_event"): { // 元事件
                        MetaEventType type = ENUM_CAST(MetaEventType, "meta_event_type");
                        MetaEvent     ev(type, packet);
                        return eventBus.publish(ev);
                    }
                    case utils::doHash("message"): { // 消息
                        MessageType    type       = ENUM_CAST(MessageType, "message_type");
                        MessageSubType subType    = ENUM_CAST(MessageSubType, "sub_type");
                        uint64_t       self       = packet["self_id"];
                        uint64_t       sender     = packet["user_id"];
                        std::string    rawMessage = packet["raw_message"];
                        int64_t        messageId  = packet["message_id"];
                        utils::ReplaceStr(rawMessage, "&#91;", "[");
                        utils::ReplaceStr(rawMessage, "&#93;", "]");
                        utils::ReplaceStr(rawMessage, "&#44;", ",");
                        utils::ReplaceStr(rawMessage, "&amp;", "&");
                        packet["raw_message"] = rawMessage;
                        std::optional<uint64_t> group;
                        if (packet.contains("group_id")) {
                            group = packet["group_id"];
                        }
                        MessageEvent ev(type, subType, self, group, sender, rawMessage, messageId, packet);
                        return eventBus.publish(ev);
                    }
                    case utils::doHash("notice"): { // 通知
                        NoticeType                   type = ENUM_CAST(NoticeType, "notice_type");
                        std::optional<NotifySubType> subType;
                        std::optional<uint64_t>      group;
                        std::optional<uint64_t>      sender;
                        std::optional<uint64_t>      self;
                        std::optional<uint64_t>      target;
                        if (packet.contains("sub_type")) {
                            subType = ENUM_CAST(NotifySubType, "sub_type");
                        }
                        if (packet.contains("group_id")) {
                            group = packet["group_id"];
                        }
                        if (packet.contains("user_id")) {
                            sender = packet["user_id"];
                        }
                        if (packet.contains("self_id")) {
                            self = packet["self_id"];
                        }
                        if (packet.contains("target_id")) {
                            target = packet["target_id"];
                        }
                        NoticeEvent ev(type, subType, group, sender, self, target, packet);
                        return eventBus.publish(ev);
                    }
                    case utils::doHash("request"): { // 请求
                        RequestType                   type    = ENUM_CAST(RequestType, "request_type");
                        uint64_t                      sender  = packet["user_id"];
                        uint64_t                      self    = packet["self_id"];
                        std::string                   comment = packet["comment"];
                        std::string                   flag    = packet["flag"];
                        std::optional<RequestSubType> subType;
                        std::optional<uint64_t>       group;
                        if (packet.contains("sub_type")) {
                            subType = ENUM_CAST(RequestSubType, "sub_type");
                        }
                        if (packet.contains("group_id")) {
                            group = packet["group_id"];
                        }
                        RequestEvent ev(type, subType, sender, self, comment, flag, group, packet);
                        return eventBus.publish(ev);
                    }
                    default: {
                        // 其它暂时视为非法信息，不解析
                        return;
                    }
                    }
                } else {
                    if (packet.contains("retcode") && packet.contains("status")) {
                        try {
                            int         retcode = packet["retcode"];
                            std::string status  = packet["status"];
                            if (retcode >= 2 && status != "ok") {
                                std::string error_message = packet["message"];
                                KobeBryant::getInstance().getLogger().error("bot.main.retError", {error_message});
                            }
                        }
                        CATCH
                    }
                    if (packet.contains("echo")) {
                        try {
                            std::string echo = packet["echo"];
                            auto        uuid = utils::UUID::fromString(echo);
                            if (mCallbacks.contains(uuid)) {
                                if (auto& call = mCallbacks[uuid]) {
                                    call(packet);
                                }
                                mCallbacks.erase(uuid);
                            }
                        }
                        CATCH
                    }
                }
            }
        } catch (const std::exception& e) {
            KobeBryant::getInstance().getLogger().error("bot.error.prasePacket", {e.what()});
        }
        CATCH_END
    });
}

void EventDispatcher::addCallback(
    const utils::UUID&                                uuid,
    std::function<void(const nlohmann::json&)> const& callback,
    std::function<void()>                             timeoutCallback,
    uint64_t                                          seconds
) {
    mCallbacks[uuid] = std::move(callback);
    Scheduler::getInstance().addDelayTask(std::chrono::milliseconds(1000 * seconds), [this, uuid, timeoutCallback] {
        std::lock_guard lock{mMutex};
        if (mCallbacks.contains(uuid) && timeoutCallback) {
            timeoutCallback();
        }
        mCallbacks.erase(uuid);
    });
}