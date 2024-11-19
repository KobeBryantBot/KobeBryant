## 概览
`PacketSender`类提供了一组方法，用于发送和处理网络数据包，特别适用于需要发送和接收消息的聊天应用程序。

- 该类不允许自己构造，需要使用PacketSender::getInstance()获取
- 示例-发送群组消息
```cpp
auto& sender = PacketSender::getInstance().sendGroupMessage();
```

## 成员函数详细说明
- ### 发送原始数据包
`sendRawPacket`函数提供了几种重载，允许以不同的方式发送原始数据包，并根据需要设置回调和超时处理。
- 重载1 用法：发送一个原始数据包，不设置回调和超时处理。
```cpp
void sendRawPacket(std::string const& rawPacket);
```
- 重载2 用法：发送一个原始数据包，并设置发送成功回调和超时回调。
```cpp
void sendRawPacket(
    std::string const& rawPacket,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 重载3 用法：发送一个以nlohmann::json对象表示的原始数据包，不设置回调和超时处理。
```cpp
void sendRawPacket(nlohmann::json const& rawPacket);
```
- 重载4 用法：发送一个以nlohmann::json对象表示的原始数据包，并设置发送成功回调和超时回调。
```cpp
void sendRawPacket(
    nlohmann::json const& rawPacket,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- rawPacket：std::string const& - 要发送的原始数据包字符串。
- rawPacket：nlohmann::json const& - 要发送的原始数据包，以nlohmann::json对象表示。
- callback：std::function<void(nlohmann::json const&)> - 发送成功后的回调函数，接收一个nlohmann::json对象作为参数。
- timeoutCallback：std::function<void()> - 超时后的回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 发送群组消息
- 重载1
```cpp
void sendGroupMessage(GroupId groupId, std::string const& msg);
```
- 重载2
```cpp
void sendGroupMessage(GroupId groupId, std::string const& msg);
```
- 参数
- groupId: GroupId - 群组ID
- msg: std::string const& - 要发送的消息内容
- ### 发送私人信息
- 重载1
```cpp
void sendPrivateMessage(UserId friendId, std::string const& msg);
```
- 重载2
```cpp
void sendPrivateMessage(UserId friendId, Message const& msg);
```
- 参数
- friendId：UserId - 好友ID
- msg：std::string const& - 要发送的消息内容。
- msg：Message const& - 要发送的消息对象。
- ### 发送好友戳一戳
```cpp
void sendFriendPoke(UserId friendId);
```
- 参数
- friendId：UserId - 好友ID。
- ### 发送群组戳一戳
```cpp
void sendGroupPoke(GroupId groupId, UserId target);
```
- 参数
- groupId：GroupId - 群组ID。
- target：UserId - 被戳的成员ID。
- ### 删除消息
```cpp
void deleteMessage(int64_t messageId);
```
- 参数
- messageId：int64_t - 要删除的消息ID。
- ### 发送点赞
```cpp
void sendLike(UserId target, uint8_t times = 10);
```
- 参数
- target：UserId - 目标用户ID。
- times：uint8_t - 点赞次数，默认为10次。
- ### 踢出群组成员
```cpp
void kickGroupMember(GroupId groupId, UserId target, bool reject = false);
```
- 参数
- groupId：GroupId - 群组ID。
- target：UserId - 被踢成员ID。
- reject：bool - 是否拒绝该成员再次加入，默认为false。
- ### 设置群成员禁言
```cpp
void setGroupMemberMute(GroupId groupId, UserId target, uint32_t duration = 30 * 60);
```
- 参数
- groupId：GroupId - 群组ID。
- target：UserId - 被禁言成员ID。
- duration：uint32_t - 禁言时长（秒），默认为30分钟。
- ### 设置群全局禁言
```cpp
void setGroupGlobalMute(GroupId groupId, bool enable = true);
```
- 参数
- groupId：GroupId - 群组ID。
- enable：bool - 是否启用全局禁言，默认为true。
- ### 设置群管理员
```cpp
void setGroupAdmin(GroupId groupId, UserId target, bool enable = true);
```
- 参数
- groupId：GroupId - 群组ID。
- target：UserId - 目标用户ID。
- enable：bool - 是否设置为管理员，默认为true。
- ### 设置群名片
```cpp
void setGroupCard(GroupId groupId, UserId target, std::string const& card);
```
- 参数
- groupId：GroupId - 群组ID。
- target：UserId - 目标用户ID。
- card：std::string const& - 群名片内容。
- ### 设置群名称
```cpp
void setGroupName(GroupId groupId, std::string const& name);
```
- 参数
- groupId：GroupId - 群组ID。
- name：std::string const& - 新的群名称。
- ### 退出群组
```cpp
void leaveGroup(GroupId groupId, bool dismiss = false);
```
- 参数
- groupId：GroupId - 群组ID。
- dismiss：bool - 是否解散群组，默认为false。
- ### 处理好友添加请求
```cpp
void handleFriendAddRequest(bool approve, std::string const& flag, std::string const& remark = {});
```
- 参数
- approve：bool - 是否批准请求。
- flag：std::string const& - 请求标识。
- remark：std::string const& - 备注信息（可选）。
- ### 处理群添加请求
```cpp
void handleGroupAddRequest(bool approve, RequestSubType type, std::string const& flag, std::string const& reason = {});
```
- 参数
- approve：bool - 是否批准请求。
- type：RequestSubType - 请求类型。
- flag：std::string const& - 请求标识。
- reason：std::string const& - 批准或拒绝的原因（可选）。
- ### 获取消息
```cpp
void getMessage(
    int64_t messageId,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- messageId：int64_t - 消息ID。
- callback：std::function<void(nlohmann::json const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 获取群列表信息
```cpp
void getGroupsListInfo(
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- callback：std::function<void(nlohmann::json const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 获取转发消息
```cpp
void getForwardMessage(
    std::string messageId,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- messageId：std::string - 消息ID。
- callback：std::function<void(nlohmann::json const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 获取登录信息
```cpp
void getLoginInfo(
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- callback：std::function<void(nlohmann::json const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 获取陌生人信息
- 重载1
```cpp
void getStrangerInfo(
    UserId target,
    bool noCache,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 重载2
```cpp
void getStrangerInfo(
    UserId target,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- target：UserId - 目标用户ID。
- noCache：bool - 是否不使用缓存。
- callback：std::function<void(nlohmann::json const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 获取好友列表信息
```cpp
void getFriendsListInfo(
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- callback：std::function<void(std::vector<UserId> const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 获取群信息
- 重载1
```cpp
void getGroupInfo(
    GroupId groupId,
    bool noCache,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 重载2
```cpp
void getGroupInfo(
    GroupId groupId,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- groupId：GroupId - 群组ID。
- noCache：bool - 是否不使用缓存。
- callback：std::function<void(nlohmann::json const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 获取群成员信息
- 重载1
```cpp
void getGroupMemberInfo(
    GroupId groupId,
    UserId target,
    bool noCache,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 重载2
```cpp
void getGroupMemberInfo(
    GroupId groupId,
    UserId target,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- groupId：GroupId - 群组ID。
- target：UserId - 目标用户ID。
- noCache：bool - 是否不使用缓存。
- callback：std::function<void(nlohmann::json const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 获取群成员列表信息
```cpp
void getGroupMembersListInfo(
    GroupId groupId,
    std::function<void(nlohmann::json const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- groupId：GroupId - 群组ID。
- callback：std::function<void(std::vector<UserId> const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
- ### 随机选择群成员
```cpp
void chooseRandomGroupMember(
    GroupId groupId,
    std::function<void(UserId)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- groupId：GroupId - 群组ID。
- callback：std::function<void(UserId)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。
### 获取群列表
```cpp
void getGroupsList(
    std::function<void(std::vector<GroupId> const&)> callback,
    std::function<void()> timeoutCallback = {},
    uint64_t seconds = 5
);
```
- 参数
- callback：std::function<void(std::vector<GroupId> const&)> - 成功回调函数。
- timeoutCallback：std::function<void()> - 超时回调函数（可选）。
- seconds：uint64_t - 超时时间（秒），默认为5秒。