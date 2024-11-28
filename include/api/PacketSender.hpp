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
    /**
     * @brief 构造一个新的消息对象
     */
    KobeBryant_NDAPI Message();

    /**
     * @brief 将消息序列化为JSON
     */
    KobeBryant_NDAPI nlohmann::json serialize() const;

    /**
     * @brief 艾特一个用户
     * @param qid 用户QQ号
     * @return 消息对象的引用
     */
    KobeBryant_API Message& at(uint64_t qid);

    /**
     * @brief 回复一个消息
     * @param msgId 被回复的消息ID
     * @return 消息对象的引用
     */
    KobeBryant_API Message& reply(int64_t msgId);

    /**
     * @brief 添加文本消息
     * @param text 文本内容
     * @return 消息对象的引用
     */
    KobeBryant_API Message& text(const std::string& text);

    /**
     * @brief 添加文本消息
     * @param fmt 格式化字符串
     * @param args 格式化参数
     * @return 消息对象的引用
     */
    template <typename... Args>
    inline Message& text(fmt::format_string<Args...> fmt, Args&&... args) {
        return text(fmt::vformat(fmt.get(), fmt::make_format_args(args...)));
    }

    /**
     * @brief 添加表情消息
     * @param id 表情ID
     * @return 消息对象的引用
     */
    KobeBryant_API Message& face(int id);

    /**
     * @brief 添加图片消息
     * @param raw 图片数据
     * @param type 图片类型
     * @param summary 图片描述
     * @return 消息对象的引用
     */
    KobeBryant_API Message&
    image(const std::string& raw, ImageType type = ImageType::Path, std::optional<std::string> summary = {});

    /**
     * @brief 添加用户头像
     * @param target 目标用户QQ号
     * @param size 头像大小
     * @param isGroup 是否是群头像
     * @return 消息对象的引用
     */
    KobeBryant_API Message& avatar(int64_t target, uint16_t size = 640, bool isGroup = false);

    /**
     * @brief 添加用户头像
     * @param target 目标用户QQ号
     * @param isGroup 是否是群头像
     * @return 消息对象的引用
     */
    KobeBryant_API Message& avatar(int64_t target, bool isGroup);

    /**
     * @brief 添加语音消息
     * @param path 语音文件路径
     * @return 消息对象的引用
     */
    KobeBryant_API Message& record(const std::filesystem::path& path);

    /**
     * @brief 添加视频消息
     * @param path 视频文件路径
     * @return 消息对象的引用
     */
    KobeBryant_API Message& video(const std::filesystem::path& path);

    /**
     * @brief 添加猜拳魔法表情
     * @return 消息对象的引用
     */
    KobeBryant_API Message& rps();

    /**
     * @brief 添加骰子魔法表情
     * @return 消息对象的引用
     */
    KobeBryant_API Message& dice();

    /**
     * @brief 添加戳一戳魔法表情
     * @return 消息对象的引用
     */
    KobeBryant_API Message& shake();

    // KobeBryant_API Message& poke(int64_t type = 1, int64_t id = -1);

    // KobeBryant_API Message& share(const std::string& url, std::optional<std::string> title = {});

    /**
     * @brief 添加名片消息
     * @param qq 名片QQ号
     * @param isGroup 是否是群名片
     * @return 消息对象的引用
     */
    KobeBryant_API Message& contact(uint64_t qq, bool isGroup = false);

    /**
     * @brief 添加JSON消息
     * @param json JSON数据
     * @return 消息对象的引用
     */
    KobeBryant_API Message& json(const std::string& json);
};

class MessagePacket {
private:
    nlohmann::json mSerializedPacket;

public:
    /**
     * @brief 构造一个新的消息包对象
     * @param target 目标QQ号
     * @param type 消息类型
     * @param msg 消息对象
     * @param echo 回包ID
     */
    KobeBryant_NDAPI explicit MessagePacket(
        uint64_t           target,
        std::string        type,
        const Message&     msg,
        const utils::UUID& echo = utils::UUID::random()
    );

    /**
     * @brief 将消息包序列化为JSON
     * @return 消息包的JSON表示
     */
    KobeBryant_NDAPI nlohmann::json serialize() const;
};

class PacketSender {
public:
    using GroupId = uint64_t;
    using UserId  = uint64_t;

    /**
     * @brief 发送一个原始数据包
     * @param rawPacket 原始数据包
     */
    KobeBryant_API static void sendRawPacket(const std::string& rawPacket);

    /**
     * @brief 发送一个原始数据包
     * @param rawPacket 原始数据包
     * @param callback 超时回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void sendRawPacket(
        const std::string&                         rawPacket,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 发送一个原始数据包
     * @param rawPacket 原始数据包
     */
    KobeBryant_API static void sendRawPacket(const nlohmann::json& rawPacket);

    /**
     * @brief 发送一个原始数据包
     * @param rawPacket 原始数据包
     * @param callback 超时回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void sendRawPacket(
        const nlohmann::json&                      rawPacket,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 发送群消息
     * @param groupId 群号
     * @param msg 消息内容
     */
    KobeBryant_API static void sendGroupMessage(GroupId groupId, const std::string& msg);

    /**
     * @brief 发送群消息
     * @param groupId 群号
     * @param msg 消息内容
     */
    KobeBryant_API static void sendGroupMessage(GroupId groupId, const Message& msg);

    /**
     * @brief 发送私聊消息
     * @param friendId 好友QQ号
     * @param msg 消息内容
     */
    KobeBryant_API static void sendPrivateMessage(UserId friendId, const std::string& msg);

    /**
     * @brief 发送私聊消息
     * @param friendId 好友QQ号
     * @param msg 消息内容
     */
    KobeBryant_API static void sendPrivateMessage(UserId friendId, const Message& msg);

    /**
     * @brief 发送好友戳一戳
     * @param friendId 好友QQ号
     */
    KobeBryant_API static void sendFriendPoke(UserId friendId);

    /**
     * @brief 发送群戳一戳
     * @param groupId 群号
     * @param target 目标QQ号
     */
    KobeBryant_API static void sendGroupPoke(GroupId groupId, UserId target);

    /**
     * @brief 撤回消息
     * @param messageId 消息ID
     */
    KobeBryant_API static void deleteMessage(int64_t messageId);

    /**
     * @brief 发送赞
     * @param target 目标QQ号
     * @param times 赞的次数
     */
    KobeBryant_API static void sendLike(UserId target, uint8_t times = 10);

    /**
     * @brief 踢出群成员
     * @param groupId 群号
     * @param target 目标QQ号
     * @param reject 是否拒绝再次加群
     */
    KobeBryant_API static void kickGroupMember(GroupId groupId, UserId target, bool reject = false);

    /**
     * @brief 禁言群成员
     * @param groupId 群号
     * @param target 目标QQ号
     * @param duration 禁言时长（秒）
     */
    KobeBryant_API static void setGroupMemberMute(GroupId groupId, UserId target, uint32_t duration = 30 * 60);

    /**
     * @brief 全体禁言
     * @param groupId 群号
     * @param enable 是否开启全体禁言
     */
    KobeBryant_API static void setGroupGlobalMute(GroupId groupId, bool enable = true);

    /**
     * @brief 设置群成员管理员
     * @param groupId 群号
     * @param target 目标QQ号
     * @param enable 是否开启管理员权限
     */
    KobeBryant_API static void setGroupAdmin(GroupId groupId, UserId target, bool enable = true);

    /**
     * @brief 设置群成员名片
     * @param groupId 群号
     * @param target 目标QQ号
     * @param card 名片
     */
    KobeBryant_API static void setGroupCard(GroupId groupId, UserId target, const std::string& card);

    /**
     * @brief 设置群名称
     * @param groupId 群号
     * @param name 群名称
     */
    KobeBryant_API static void setGroupName(GroupId groupId, const std::string& name);

    /**
     * @brief 退出群聊
     * @param groupId 群号
     * @param dismiss 是否解散群聊
     */
    KobeBryant_API static void leaveGroup(GroupId groupId, bool dismiss = false);

    /**
     * @brief 处理好友添加请求
     * @param approve 是否同意添加请求
     * @param flag 请求标识（从好友请求的事件获取）
     * @param remark 备注
     */
    KobeBryant_API static void
    handleFriendAddRequest(bool approve, const std::string& flag, const std::string& remark = {});

    /**
     * @brief 处理群添加请求
     * @param approve 是否同意添加请求
     * @param type 请求类型
     * @param flag 请求标识（从群请求的事件获取）
     * @param reason 原因
     */
    KobeBryant_API static void
    handleGroupAddRequest(bool approve, RequestSubType type, const std::string& flag, const std::string& reason = {});

    /**
     * @brief 获取消息数据
     * @param messageId 消息ID
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getMessage(
        int64_t                                    messageId,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取群列表信息
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getGroupsListInfo(
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取合并转发消息
     * @param messageId 消息ID
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getForwardMessage(
        std::string                                messageId,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取当前登录信息
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getLoginInfo(
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取陌生人信息
     * @param target 目标QQ号
     * @param noCache 是否不使用缓存
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getStrangerInfo(
        UserId                                     target,
        bool                                       noCache,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取陌生人信息
     * @param target 目标QQ号
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getStrangerInfo(
        UserId                                     target,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取好友列表信息
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getFriendsListInfo(
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取好友列表
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getFriendsList(
        std::function<void(const std::vector<UserId>&)> callback,
        std::function<void()>                           timeoutCallback = {},
        uint64_t                                        seconds         = 5
    );

    /**
     * @brief 获取群信息
     * @param groupId 群号
     * @param noCache 是否不使用缓存
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getGroupInfo(
        GroupId                                    groupId,
        bool                                       noCache,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取群信息
     * @param groupId 群号
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getGroupInfo(
        GroupId                                    groupId,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取群成员信息
     * @param groupId 群号
     * @param target 目标QQ号
     * @param noCache 是否不使用缓存
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getGroupMemberInfo(
        GroupId                                    groupId,
        UserId                                     target,
        bool                                       noCache,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取群成员信息
     * @param groupId 群号
     * @param target 目标QQ号
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getGroupMemberInfo(
        GroupId                                    groupId,
        UserId                                     target,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取群成员列表信息
     * @param groupId 群号
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getGroupMembersListInfo(
        GroupId                                    groupId,
        std::function<void(const nlohmann::json&)> callback,
        std::function<void()>                      timeoutCallback = {},
        uint64_t                                   seconds         = 5
    );

    /**
     * @brief 获取群成员列表
     * @param groupId 群号
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getGroupMembersList(
        GroupId                                         groupId,
        std::function<void(const std::vector<UserId>&)> callback,
        std::function<void()>                           timeoutCallback = {},
        uint64_t                                        seconds         = 5
    );

    /**
     * @brief 随机选择群成员
     * @param groupId 群号
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void chooseRandomGroupMember(
        GroupId                     groupId,
        std::function<void(UserId)> callback,
        std::function<void()>       timeoutCallback = {},
        uint64_t                    seconds         = 5
    );

    /**
     * @brief 获取群列表
     * @param callback 回调函数
     * @param timeoutCallback 超时回调函数
     * @param seconds 超时时间（秒）
     */
    KobeBryant_API static void getGroupsList(
        std::function<void(const std::vector<GroupId>&)> callback,
        std::function<void()>                            timeoutCallback = {},
        uint64_t                                         seconds         = 5
    );
};
