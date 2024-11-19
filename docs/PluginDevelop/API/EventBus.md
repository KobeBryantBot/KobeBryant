## EventBus.hpp

## 概览
`EventBus`类是一个事件总线系统，用于在不同的组件或模块之间发布和订阅事件。它支持事件的注册、注销和分发。
- 该类不允许自己构造，需要使用PacketSender::getInstance()获取

## 成员函数详细说明

### 单例模式 
用法：获取EventBus类的单例实例。
```cpp
static EventBus& getInstance();
```
-返回值
- EventBus& - 返回EventBus类的单例实例。

### 订阅事件 
用法：订阅一个特定类型的事件。
```cpp
template <std::derived_from<Event> T>
Listener subscribe(std::function<void(T&)> callback, uint32_t priority = 500);
```
- 参数：
- callback：std::function<void(T&)> - 当事件被触发时调用的回调函数。
- priority：uint32_t - 订阅者的优先级，默认为500。
- 返回值：
- Listener - 返回一个Listener对象，代表这个订阅。

### 取消订阅 
用法：取消之前订阅的事件
```cpp
bool unsubscribe(Listener const& listener);
```
- 参数：
- listener：Listener const& - 要取消订阅的Listener对象。
- 返回值：
- bool - 表示是否成功取消订阅。

### 发布事件 
用法：发布一个事件，所有订阅了该事件的回调函数将被调用。
```cpp
template <std::derived_from<Event> T>
void publish(T& ev);
```
- 参数：
- ev：T& - 要发布的事件对象。
