#pragma once
#include "Enums.hpp"
#include "Macros.hpp"
#include "api/utils/UUID.hpp"
#include "fmt/format.h"
#include "nlohmann/json.hpp"
#include <chrono>

class Message {
private:
    nlohmann::json mSerializedMessage;

public:
    enum class ImageType : int {
        Path   = 0, // 从文件路径读取
        Binary = 1, // 二进制数据
        Url    = 2, // 网络URL
        Base64 = 3  // Bsae64
    };

public:
    KobeBryant_NDAPI Message();

    KobeBryant_NDAPI nlohmann::json serialize() const;

    KobeBryant_API Message& at(uint64_t qid);

    KobeBryant_API Message& reply(int64_t msgId);

    KobeBryant_API Message& text(std::string const& text);

    template <typename... Args>
    inline Message& text(fmt::format_string<Args...> fmt, Args&&... args) {
        return text(fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }

    KobeBryant_API Message& face(int id);

    KobeBryant_API Message&
    image(std::string const& raw, ImageType type = ImageType::Path, std::optional<std::string> summary = {});

    KobeBryant_API Message& avatar(int64_t target, uint16_t size = 640, bool isGroup = false);

    KobeBryant_API Message& avatar(int64_t target, bool isGroup);

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

class PacketSender {
public:
    PacketSender();

    PacketSender(const PacketSender&)            = delete;
    PacketSender& operator=(const PacketSender&) = delete;

    using GroupId = uint64_t;
    using UserId  = uint64_t;

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

    KobeBryant_API void sendGroupMessage(GroupId groupId, std::string const& msg);

    KobeBryant_API void sendGroupMessage(GroupId groupId, Message const& msg);

    KobeBryant_API void sendPrivateMessage(UserId friendId, std::string const& msg);

    KobeBryant_API void sendPrivateMessage(UserId friendId, Message const& msg);

    KobeBryant_API void sendFriendPoke(UserId friendId);

    KobeBryant_API void sendGroupPoke(GroupId groupId, UserId target);

    KobeBryant_API void deleteMessage(int64_t messageId);

    KobeBryant_API void sendLike(UserId target, uint8_t times = 10);

    KobeBryant_API void kickGroupMember(GroupId groupId, UserId target, bool reject = false);

    KobeBryant_API void setGroupMemberMute(GroupId groupId, UserId target, uint32_t duration = 30 * 60);

    KobeBryant_API void setGroupGlobalMute(GroupId groupId, bool enable = true);

    KobeBryant_API void setGroupAdmin(GroupId groupId, UserId target, bool enable = true);

    KobeBryant_API void setGroupCard(GroupId groupId, UserId target, std::string const& card);

    KobeBryant_API void setGroupName(GroupId groupId, std::string const& name);

    KobeBryant_API void leaveGroup(GroupId groupId, bool dismiss = false);

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
        UserId                                     target,
        bool                                       noCache,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getStrangerInfo(
        UserId                                     target,
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
        std::function<void(std::vector<UserId> const&)> callback,
        std::function<void()>                           timeoutCallback = {},
        uint64_t                                        seconds         = 5
    );

    KobeBryant_API void getGroupInfo(
        GroupId                                    groupId,
        bool                                       noCache,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupInfo(
        GroupId                                    groupId,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupMemberInfo(
        GroupId                                    groupId,
        UserId                                     target,
        bool                                       noCache,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupMemberInfo(
        GroupId                                    groupId,
        UserId                                     target,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupMembersListInfo(
        GroupId                                    groupId,
        std::function<void(nlohmann::json const&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    KobeBryant_API void getGroupMembersList(
        GroupId                                         groupId,
        std::function<void(std::vector<UserId> const&)> callback,
        std::function<void()>                           timeoutCallback = {},
        uint64_t                                        seconds         = 5
    );

    KobeBryant_API void chooseRandomGroupMember(
        GroupId                     groupId,
        std::function<void(UserId)> callback,
        std::function<void()>       timeoutCallback = {},
        uint64_t                    seconds         = 5
    );

    KobeBryant_API void getGroupsList(
        std::function<void(std::vector<GroupId> const&)> callback,
        std::function<void()>                            timeoutCallback = {},
        uint64_t                                         seconds         = 5
    );
};
