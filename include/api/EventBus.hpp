#pragma once
#include "Macros.hpp"
#include "api/event/Event.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <chrono>
#include <concepts>
#include <functional>
#include <iostream>
#include <map>
#include <typeindex>

#define EVENT_CALLBACK                                                                                                 \
    [=](Event& event) {                                                                                                \
        try {                                                                                                          \
            if (callback) {                                                                                            \
                E& ev = dynamic_cast<E&>(event);                                                                       \
                callback(ev);                                                                                          \
            }                                                                                                          \
        } catch (const std::exception& e) {                                                                            \
            printException(e.what());                                                                                  \
        }                                                                                                              \
    }

struct Listener {
    uint64_t              mId;
    std::type_index       mType;
    std::string           mPlugin;
    KobeBryant_NDAPI      Listener(std::type_index type, const std::string& plugin);
    KobeBryant_NDAPI bool operator<(const Listener& rhs) const;
    KobeBryant_NDAPI bool operator==(const Listener& rhs) const;
};

class EventBus {
public:
    /**
     * 订阅一个事件
     * @param callback 事件发生时要执行的回调函数
     * @param priority 监听事件的优先级，默认为500
     * @return 返回创建的监听器
     */
    template <std::derived_from<Event> E>
    static inline Listener subscribe(std::function<void(E&)> callback, uint32_t priority = 500) {
        return addListener(std::type_index(typeid(E)), utils::getCurrentPluginName(), EVENT_CALLBACK, priority);
    }

    /**
     * 临时订阅一个事件
     * @param duration 事件监听的持续时间，超时后取消订阅
     * @param callback 事件发生时要执行的回调函数
     * @param priority 监听事件的优先级，默认为500
     * @return 返回创建的监听器
     */
    template <std::derived_from<Event> E, class T, class D>
    static inline Listener
    subscribeTemp(std::chrono::duration<T, D> duration, std::function<void(E&)> callback, uint32_t priority = 500) {
        return addListenerTemp(
            std::type_index(typeid(E)),
            utils::getCurrentPluginName(),
            EVENT_CALLBACK,
            priority,
            std::chrono::duration_cast<std::chrono::milliseconds>(duration)
        );
    }

    /**
     * 临时订阅一个事件
     * @param times 事件监听后的监听次数，事件触发超过此数后取消订阅
     * @param callback 事件发生时要执行的回调函数
     * @param priority 监听事件的优先级，默认为500
     * @return 返回创建的监听器
     */
    template <std::derived_from<Event> E>
    static inline Listener subscribeTemp(size_t times, std::function<void(E&)> callback, uint32_t priority = 500) {
        return addListenerTemp(
            std::type_index(typeid(E)),
            utils::getCurrentPluginName(),
            EVENT_CALLBACK,
            priority,
            times
        );
    }

    /**
     * 临时订阅一个事件
     * @param duration 事件监听的持续时间，超时后取消订阅
     * @param times 事件监听后的监听次数，事件触发超过此数后取消订阅
     * @param callback 事件发生时要执行的回调函数
     * @param priority 监听事件的优先级，默认为500
     * @return 返回创建的监听器
     */
    template <std::derived_from<Event> E, class T, class D>
    static inline Listener subscribeTemp(
        std::chrono::duration<T, D> duration,
        size_t                      times,
        std::function<void(E&)>     callback,
        uint32_t                    priority = 500
    ) {
        return addListenerTemp(
            std::type_index(typeid(E)),
            utils::getCurrentPluginName(),
            EVENT_CALLBACK,
            priority,
            std::chrono::duration_cast<std::chrono::milliseconds>(duration),
            times
        );
    }

    /**
     * 取消订阅一个事件
     * @param listener 要取消订阅的监听器对象
     * @return 如果成功取消订阅返回 true，否则返回 false
     */
    static inline bool unsubscribe(const Listener& listener) {
        return removeListener(utils::getCurrentPluginName(), listener);
    }

    /**
     * 发布一个事件
     * @param ev 要发布的事件对象
     */
    template <std::derived_from<Event> T>
    static inline void publish(T& ev) {
        forEachListener(std::type_index(typeid(T)), [&](const std::function<void(Event&)>& callback) -> bool {
            try {
                callback(ev);
                return !ev.isPassingBlocked();
            } catch (const std::exception& e) {
                printException(e.what());
                return false;
            }
        });
    }

protected:
    KobeBryant_NDAPI static Listener
    addListener(std::type_index, const std::string&, std::function<void(Event&)>, uint32_t);
    KobeBryant_NDAPI static Listener addListenerTemp(
        std::type_index,
        const std::string&,
        std::function<void(Event&)>,
        uint32_t,
        std::chrono::milliseconds
    );
    KobeBryant_NDAPI static Listener
    addListenerTemp(std::type_index, const std::string&, std::function<void(Event&)>, uint32_t, size_t);
    KobeBryant_NDAPI static Listener addListenerTemp(
        std::type_index,
        const std::string&,
        std::function<void(Event&)>,
        uint32_t,
        std::chrono::milliseconds,
        size_t
    );
    KobeBryant_API static void
        forEachListener(std::type_index, std::function<bool(const std::function<void(Event&)>&)>);
    KobeBryant_API static bool removeListener(const std::string&, const Listener&);
    KobeBryant_API static void printException(const std::string&);
};