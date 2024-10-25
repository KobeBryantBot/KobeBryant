#pragma once

enum class GroupPermissionType : int {
    Owner  = 0, // 群主
    Admin  = 1, // 管理员
    Member = 2  // 普通成员
};

enum class MessageType : int {
    Private = 0, // 私聊消息
    Group   = 1  // 群消息
};

enum class MessageSubType : int {
    Friend    = 0, // 好友
    Normal    = 1, // 群聊
    Anonymous = 2, // 匿名
    GroupSelf = 3, // 群中自身发送
    Group     = 4, // 群临时会话
    Notice    = 5, // 系统提示
};

enum class MetaEventType : int {
    Lifecycle = 0, // 生命周期
    Heartbeat = 1  // 心跳包
};

enum class NoticeType : int {
    GroupUpload   = 0,  // 群文件上传
    GroupAdmin    = 1,  // 群管理员变更
    GroupDecrease = 2,  // 群成员减少
    GroupIncrease = 3,  // 群成员增加
    GroupBan      = 4,  // 群成员禁言
    FriendAdd     = 5,  // 好友添加
    GroupRecall   = 6,  // 群消息撤回
    FriendRecall  = 7,  // 好友消息撤回
    GroupCard     = 8,  // 群名片变更
    OfflineFile   = 9,  // 离线文件上传
    ClientStatus  = 10, // 客户端状态变更
    Essence       = 11, // 精华消息
    Notify        = 12, // 系统通知
};

enum class NotifySubType : int {
    Honor     = 0, // 群荣誉变更
    Poke      = 1, // 戳一戳
    LuckyKing = 2, // 群红包幸运王
    Title     = 3  // 群成员头衔变更
};

enum class RequestType : int {
    Friend = 0, // 好友请求
    Group  = 1  // 加群请求
};

enum class RequestSubType : int {
    Add    = 0, // 正常加群
    Invite = 1  // 邀请加群
};