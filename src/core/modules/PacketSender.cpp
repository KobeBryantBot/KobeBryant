#include "api/PacketSender.hpp"
#include "EventDispatcher.hpp"
#include "KobeBryant.hpp"
#include "api/utils/Base64Utils.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/Global.hpp"
#include "magic_enum/magic_enum.hpp"
#include <random>

Message::Message() : mSerializedMessage(nlohmann::json::array()) {}

Message& Message::at(uint64_t qid) {
    mSerializedMessage.push_back({
        {"type", "at"         },
        {"data", {{"qq", qid}}}
    });
    return *this;
}

Message& Message::text(std::string const& text) {
    mSerializedMessage.push_back({
        {"type", "text"          },
        {"data", {{"text", text}}}
    });
    return *this;
}

Message& Message::face(int id) {
    mSerializedMessage.push_back({
        {"type", "face"      },
        {"data", {{"id", id}}}
    });
    return *this;
}

Message& Message::reply(int64_t id) {
    mSerializedMessage.push_back({
        {"type", "reply"     },
        {"data", {{"id", id}}}
    });
    return *this;
}

Message& Message::image(std::string const& raw, ImageType type, bool flash, std::optional<std::string> summary) {
    try {
        nlohmann::json json = {
            {"type", "image"         },
            {"data", {{"subType", 0}}}
        };
        if (type == ImageType::Binary) {
            auto info    = "base64://" + utils::encode(raw);
            json["file"] = info;
        } else if (type == ImageType::Path) {
            auto info    = "file://" + std::filesystem::absolute(raw).string();
            json["file"] = info;
        }
        if (flash) {
            json["type"] = "flash";
        }
        if (summary) {
            json["data"]["summary"] = *summary;
        }
        mSerializedMessage.push_back(json);
    } catch (...) {}
    return *this;
}

Message& Message::image(std::string const& raw, ImageType type, std::optional<std::string> summary) {
    return image(raw, type, false, summary);
}

Message& Message::record(std::filesystem::path const& path) {
    mSerializedMessage.push_back({
        {"type", "record"                                                        },
        {"data", {{"file", "file://" + std::filesystem::absolute(path).string()}}}
    });
    return *this;
}

Message& Message::video(std::filesystem::path const& path) {
    mSerializedMessage.push_back({
        {"type", "video"                                                         },
        {"data", {{"file", "file://" + std::filesystem::absolute(path).string()}}}
    });
    return *this;
}

// 猜拳魔法表情
Message& Message::rps() {
    mSerializedMessage.push_back({
        {"type", "rps"},
        {"data", {}   }
    });
    return *this;
}

// 掷骰子魔法表情
Message& Message::dice() {
    mSerializedMessage.push_back({
        {"type", "dice"},
        {"data", {}    }
    });
    return *this;
}

Message& Message::shake() {
    mSerializedMessage.push_back({
        {"type", "shake"},
        {"data", {}     }
    });
    return *this;
}

/*
Message& Message::poke(int64_t type, int64_t id) {
    mSerializedMessage.push_back({
        {"type", "poke"                            },
        {"data", {{"type", S(type)}, {"id", S(id)}}}
    });
    return *this;
}

Message& Message::share(std::string const& url, std::optional<std::string> title) {
    nlohmann::json json = {
        {"type", "share"       },
        {"data", {{"url", url}}}
    };
    if (title) {
        json["data"]["title"] = *title;
    }
    mSerializedMessage.push_back(json);
    return *this;
}
*/

Message& Message::contact(uint64_t qq, bool isGroup) {
    mSerializedMessage.push_back({
        {"type", "contact"                                       },
        {"data", {{"type", isGroup ? "group" : "qq"}, {"id", qq}}}
    });
    return *this;
}

Message& Message::json(std::string const& json) {
    mSerializedMessage.push_back({
        {"type", "contact"       },
        {"data", {{"data", json}}}
    });
    return *this;
}

nlohmann::json Message::serialize() const { return mSerializedMessage; }

MessagePacket::MessagePacket(uint64_t target, std::string type, Message const& msg, utils::UUID const& echo) {
    mSerializedPacket = {
        {"action", "send_msg"                                                                 },
        {"echo",   echo.toString()                                                            },
        {"params", {{"user_id", target}, {"message", msg.serialize()}, {"message_type", type}}}
    };
    if (type == "group") {
        mSerializedPacket["params"]["group_id"] = target;
    }
}

nlohmann::json MessagePacket::serialize() const { return mSerializedPacket; }

PacketSender::PacketSender() {}

PacketSender& PacketSender::getInstance() {
    static std::unique_ptr<PacketSender> instance;
    if (!instance) {
        instance = std::make_unique<PacketSender>();
    }
    return *instance;
}

void PacketSender::sendRawPacket(std::string const& rawPacket) {
    if (KobeBryant::getInstance().hasConnected()) {
        KobeBryant::getInstance().sendRawPacket(rawPacket);
    }
}

void PacketSender::sendRawPacket(nlohmann::json const& rawPacket) { sendRawPacket(rawPacket.dump()); }

void PacketSender::sendRawPacket(
    std::string const&                         rawPacket,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    auto json = nlohmann::json::parse(rawPacket);
    return sendRawPacket(std::move(json), std::move(callback), std::move(timeoutCallback), seconds);
}

void PacketSender::sendRawPacket(
    nlohmann::json const&                      rawPacket,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    if (rawPacket.contains("echo") && callback) {
        // 存在 echo 才能绑定
        std::string echo = rawPacket["echo"];
        auto        uuid = utils::UUID::fromString(echo);
        EventDispatcher::getInstance()
            .addCallback(std::move(uuid), std::move(callback), std::move(timeoutCallback), seconds);
    }
    return sendRawPacket(rawPacket);
}

void PacketSender::sendGroupMessage(uint64_t groupId, std::string const& msg) {
    auto message = Message().text(msg);
    sendGroupMessage(groupId, message);
}

void PacketSender::sendGroupMessage(uint64_t groupId, Message const& msg) {
    auto pkt = MessagePacket(groupId, "group", msg);
    sendRawPacket(pkt.serialize());
}

void PacketSender::sendPrivateMessage(uint64_t friendId, std::string const& msg) {
    auto message = Message().text(msg);
    sendPrivateMessage(friendId, message);
}

void PacketSender::sendPrivateMessage(uint64_t friendId, Message const& msg) {
    auto pkt = MessagePacket(friendId, "private", msg);
    sendRawPacket(pkt.serialize());
}

void PacketSender::sendFriendPoke(uint64_t friendId) {
    nlohmann::json packet = {
        {"action", "friend_poke"          },
        {"params", {{"user_id", friendId}}}
    };
    sendRawPacket(packet);
}

void PacketSender::sendGroupPoke(uint64_t groupId, uint64_t target) {
    nlohmann::json packet = {
        {"action", "group_poke"                                },
        {"params", {{"group_id", groupId}, {"user_id", target}}}
    };
    sendRawPacket(packet);
}

void PacketSender::deleteMessage(int64_t messageId) {
    nlohmann::json packet = {
        {"action", "delete_msg"               },
        {"params", {{"message_id", messageId}}}
    };
    sendRawPacket(packet);
}

void PacketSender::sendLike(uint64_t target, uint8_t times) {
    nlohmann::json packet = {
        {"action", "send_like"                            },
        {"params", {{"user_id", target}, {"times", times}}}
    };
    sendRawPacket(packet);
}

void PacketSender::kickGroupMember(uint64_t groupId, uint64_t target, bool reject) {
    nlohmann::json packet = {
        {"action", "set_group_kick"                                                            },
        {"params", {{"group_id", groupId}, {"user_id", target}, {"reject_add_request", reject}}}
    };
    sendRawPacket(packet);
}

void PacketSender::setGroupMemberMute(uint64_t groupId, uint64_t target, uint32_t duration) {
    nlohmann::json packet = {
        {"action", "set_group_ban"                                                     },
        {"params", {{"group_id", groupId}, {"user_id", target}, {"duration", duration}}}
    };
    sendRawPacket(packet);
}

void PacketSender::setGroupGlobalMute(uint64_t groupId, bool enable) {
    nlohmann::json packet = {
        {"action", "set_group_whole_ban"                      },
        {"params", {{"group_id", groupId}, {"enable", enable}}}
    };
    sendRawPacket(packet);
}

void PacketSender::setGroupAdmin(uint64_t groupId, uint64_t target, bool enable) {
    nlohmann::json packet = {
        {"action", "set_group_admin"                                               },
        {"params", {{"group_id", groupId}, {"user_id", target}, {"enable", enable}}}
    };
    sendRawPacket(packet);
}

void PacketSender::setGroupCard(uint64_t groupId, uint64_t target, std::string const& card) {
    nlohmann::json packet = {
        {"action", "set_group_card"                                            },
        {"params", {{"group_id", groupId}, {"user_id", target}, {"card", card}}}
    };
    sendRawPacket(packet);
}

void PacketSender::setGroupName(uint64_t groupId, std::string const& name) {
    nlohmann::json packet = {
        {"action", "set_group_card"                             },
        {"params", {{"group_id", groupId}, {"group_name", name}}}
    };
    sendRawPacket(packet);
}

void PacketSender::leaveGroup(uint64_t groupId, bool dismiss) {
    nlohmann::json packet = {
        {"action", "set_group_leave"                               },
        {"params", {{"group_id", groupId}, {"is_dismiss", dismiss}}}
    };
    sendRawPacket(packet);
}

void PacketSender::handleFriendAddRequest(bool approve, std::string const& flag, std::string const& remark) {
    nlohmann::json packet = {
        {"action", "set_friend_add_request"                                  },
        {"params", {{"flag", flag}, {"approve", approve}, {"remark", remark}}}
    };
    sendRawPacket(packet);
}

void PacketSender::handleGroupAddRequest(
    bool               approve,
    RequestSubType     type,
    std::string const& flag,
    std::string const& reason
) {
    nlohmann::json packet = {
        {"action", "set_group_add_request"},
        {"params",
         {{"flag", flag},
          {"sub_type", utils::toSnakeCase(magic_enum::enum_name(type))},
          {"approve", approve},
          {"reason", reason}}             }
    };
    sendRawPacket(packet);
}

void PacketSender::getMessage(
    int64_t                                    messageId,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_msg"                       },
        {"echo",   utils::UUID::random().toString()},
        {"params", {{"message_id", messageId}}     }
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getGroupsListInfo(
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_group_list"                },
        {"echo",   utils::UUID::random().toString()},
        {"params", {}                              }
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getForwardMessage(
    std::string                                messageId,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_forward_msg"               },
        {"echo",   utils::UUID::random().toString()},
        {"params", {{"id", messageId}}             }
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getLoginInfo(
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_login_info"                },
        {"echo",   utils::UUID::random().toString()},
        {"params", {}                              }
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getStrangerInfo(
    uint64_t                                   target,
    bool                                       noCache,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_stranger_info"                         },
        {"echo",   utils::UUID::random().toString()            },
        {"params", {{"user_id", target}, {"no_cache", noCache}}}
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getStrangerInfo(
    uint64_t                                   target,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    return getStrangerInfo(target, false, callback, timeoutCallback, seconds);
}

void PacketSender::getFriendsListInfo(
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_friend_list"               },
        {"echo",   utils::UUID::random().toString()},
        {"params", {}                              }
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getFriendsList(
    std::function<void(std::vector<uint64_t> const&)> callback,
    std::function<void()>                             timeoutCallback,
    uint64_t                                          seconds
) {
    return getFriendsListInfo(
        [=](nlohmann::json const& packet) {
            try {
                if (callback) {
                    auto                  data = packet["data"];
                    std::vector<uint64_t> list;
                    for (auto& info : data) {
                        uint64_t qid = info["user_id"];
                        list.push_back(qid);
                    }
                    callback(list);
                }
            } catch (...) {}
        },
        timeoutCallback,
        seconds
    );
}

void PacketSender::getGroupInfo(
    uint64_t                                   groupId,
    bool                                       noCache,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_group_info"                              },
        {"echo",   utils::UUID::random().toString()              },
        {"params", {{"group_id", groupId}, {"no_cache", noCache}}}
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getGroupInfo(
    uint64_t                                   groupId,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    return getGroupInfo(groupId, false, callback, timeoutCallback, seconds);
}

void PacketSender::getGroupMemberInfo(
    uint64_t                                   groupId,
    uint64_t                                   target,
    bool                                       noCache,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_group_member_info"                                            },
        {"echo",   utils::UUID::random().toString()                                   },
        {"params", {{"group_id", groupId}, {"user_id", target}, {"no_cache", noCache}}}
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getGroupMemberInfo(
    uint64_t                                   groupId,
    uint64_t                                   target,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    return getGroupMemberInfo(groupId, target, false, callback, timeoutCallback, seconds);
}

void PacketSender::getGroupMembersListInfo(
    uint64_t                                   groupId,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()>                      timeoutCallback,
    uint64_t                                   seconds
) {
    nlohmann::json packet = {
        {"action", "get_group_member_list"         },
        {"echo",   utils::UUID::random().toString()},
        {"params", {{"group_id", groupId}}         }
    };
    sendRawPacket(packet, callback, timeoutCallback, seconds);
}

void PacketSender::getGroupMembersList(
    uint64_t                                          groupId,
    std::function<void(std::vector<uint64_t> const&)> callback,
    std::function<void()>                             timeoutCallback,
    uint64_t                                          seconds
) {
    return getGroupMembersListInfo(
        groupId,
        [=](nlohmann::json const& packet) {
            try {
                if (callback) {
                    auto                  data = packet["data"];
                    std::vector<uint64_t> list;
                    for (auto& info : data) {
                        uint64_t qid = info["user_id"];
                        list.push_back(qid);
                    }
                    callback(list);
                }
            } catch (...) {}
        },
        timeoutCallback,
        seconds
    );
}

void PacketSender::chooseRandomGroupMember(
    uint64_t                      groupId,
    std::function<void(uint64_t)> callback,
    std::function<void()>         timeoutCallback,
    uint64_t                      seconds
) {
    return getGroupMembersList(
        groupId,
        [=](std::vector<uint64_t> const& list) {
            if (callback) {
                std::random_device              rd;
                std::mt19937                    gen(rd());
                std::uniform_int_distribution<> dis(0, list.size() - 1);
                auto                            index  = dis(gen);
                auto                            target = list[index];
                callback(target);
            }
        },
        timeoutCallback,
        seconds
    );
}

void PacketSender::getGroupsList(
    std::function<void(std::vector<uint64_t> const&)> callback,
    std::function<void()>                             timeoutCallback,
    uint64_t                                          seconds
) {
    return getGroupsListInfo(
        [=](nlohmann::json const& packet) {
            try {
                if (callback) {
                    auto                  data = packet["data"];
                    std::vector<uint64_t> list;
                    for (auto& info : data) {
                        uint64_t gid = info["group_id"];
                        list.push_back(gid);
                    }
                    callback(list);
                }
            } catch (...) {}
        },
        timeoutCallback,
        seconds
    );
}