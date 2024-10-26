#pragma once
#include "Enums.hpp"
#include "Macros.hpp"
#include "api/utils/UUID.hpp"
#include "nlohmann/json.hpp"
#include <chrono>

class Message {
private:
    nlohmann::json mSerializedMessage;

public:
    enum class ImageType : int {
        Path   = 0, // 从文件路径读取
        Binary = 1, // 二进制数据
    };

public:
    KobeBryant_NDAPI Message();

    KobeBryant_NDAPI nlohmann::json serialize() const;

    KobeBryant_API Message& at(uint64_t qid);

    KobeBryant_API Message& reply(int64_t msgId);

    KobeBryant_API Message& text(std::string const& text);

    KobeBryant_API Message& face(int id);

    KobeBryant_API Message& image(
        std::string const&         raw,
        ImageType                  type    = ImageType::Path,
        bool                       flash   = false,
        std::optional<std::string> summary = {}
    );

    KobeBryant_API Message&
    image(std::string const& raw, ImageType type = ImageType::Path, std::optional<std::string> summary = {});

    KobeBryant_API Message& record(std::filesystem::path const& path);

    KobeBryant_API Message& video(std::filesystem::path const& path);

    // 猜拳魔法表情
    KobeBryant_API Message& rps();

    // 掷骰子魔法表情
    KobeBryant_API Message& dice();

    KobeBryant_API Message& shake();

    // KobeBryant_API Message& poke(int64_t type = 1, int64_t id = -1);

    // KobeBryant_API Message& share(std::string const& url, std::optional<std::string> title = {});

    KobeBryant_API Message& contact(uint64_t qq, bool isGroup = false);

    KobeBryant_API Message& json(std::string const& json);
};

class MessagePacket {
private:
    nlohmann::json mSerializedPacket;

public:
    KobeBryant_NDAPI explicit MessagePacket(
        uint64_t           target,
        std::string        type,
        Message const&     msg,
        utils::UUID const& echo = utils::UUID::random()
    );

    KobeBryant_NDAPI nlohmann::json serialize() const;
};

using namespace std::chrono_literals;

class PacketSender {
public:
    PacketSender();

    PacketSender(const PacketSender&)            = delete;
    PacketSender& operator=(const PacketSender&) = delete;

    KobeBryant_NDAPI static PacketSender& getInstance();

    KobeBryant_API void sendRawPacket(std::string const& rawPacket);

    KobeBryant_API void sendRawPacket(
        std::string const&                         rawPacket,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void sendRawPacket(nlohmann::json const& rawPacket);

    KobeBryant_API void sendRawPacket(
        nlohmann::json const&                      rawPacket,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void sendGroupMessage(uint64_t groupId, std::string const& msg);

    KobeBryant_API void sendGroupMessage(uint64_t groupId, Message const& msg);

    KobeBryant_API void sendPrivateMessage(uint64_t friendId, std::string const& msg);

    KobeBryant_API void sendPrivateMessage(uint64_t friendId, Message const& msg);

    KobeBryant_API void sendFriendPoke(uint64_t friendId);

    KobeBryant_API void sendGroupPoke(uint64_t groupId, uint64_t target);

    KobeBryant_API void deleteMessage(int64_t messageId);

    KobeBryant_API void sendLike(uint64_t target, uint8_t times = 10);

    KobeBryant_API void kickGroupMember(uint64_t groupId, uint64_t target, bool reject = false);

    KobeBryant_API void setGroupMemberMute(uint64_t groupId, uint64_t target, uint32_t duration = 30 * 60);

    KobeBryant_API void setGroupGlobalMute(uint64_t groupId, bool enable = true);

    KobeBryant_API void setGroupAdmin(uint64_t groupId, uint64_t target, bool enable = true);

    KobeBryant_API void setGroupCard(uint64_t groupId, uint64_t target, std::string const& card);

    KobeBryant_API void setGroupName(uint64_t groupId, std::string const& name);

    KobeBryant_API void leaveGroup(uint64_t groupId, bool dismiss = false);

    KobeBryant_API void handleFriendAddRequest(bool approve, std::string const& flag, std::string const& remark = {});

    KobeBryant_API void
    handleGroupAddRequest(bool approve, RequestSubType type, std::string const& flag, std::string const& reason = {});

    KobeBryant_API void getMessage(
        int64_t                                    messageId,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupsListInfo(
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getForwardMessage(
        std::string                                messageId,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getLoginInfo(
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getStrangerInfo(
        uint64_t                                   target,
        bool                                       noCache,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getStrangerInfo(
        uint64_t                                   target,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getFriendsListInfo(
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getFriendsList(
        std::function<void(std::vector<uint64_t> const&)> callback,
        std::function<void()>                             timeoutCallback = {},
        uint64_t                                          seconds         = 5
    );

    KobeBryant_API void getGroupInfo(
        uint64_t                                   groupId,
        bool                                       noCache,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupInfo(
        uint64_t                                   groupId,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupMemberInfo(
        uint64_t                                   groupId,
        uint64_t                                   target,
        bool                                       noCache,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupMemberInfo(
        uint64_t                                   groupId,
        uint64_t                                   target,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupMembersListInfo(
        uint64_t                                   groupId,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupMembersList(
        uint64_t                                          groupId,
        std::function<void(std::vector<uint64_t> const&)> callback,
        std::function<void()>                             timeoutCallback = {},
        uint64_t                                          seconds         = 5
    );

    KobeBryant_API void chooseRandomGroupMember(
        uint64_t                      groupId,
        std::function<void(uint64_t)> callback,
        std::function<void()>         timeoutCallback = {},
        uint64_t                      seconds         = 5
    );

    KobeBryant_API void getGroupsList(
        std::function<void(std::vector<uint64_t> const&)> callback,
        std::function<void()>                             timeoutCallback = {},
        uint64_t                                          seconds         = 5
    );
};
