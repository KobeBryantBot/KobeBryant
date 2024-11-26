#include "KobeBryant.hpp"
#include "api/Logger.hpp"
#include "core/modules/EventBusImpl.hpp"
#include "core/modules/ScheduleManager.hpp"

bool Listener::operator<(const Listener& rhs) const { return mId < rhs.mId; }

bool Listener::operator==(const Listener& rhs) const { return mId == rhs.mId; }

Listener::Listener(std::type_index type, const std::string& plugin) : mType(type), mPlugin(plugin) {
    auto& impl = EventBusImpl::getInstance();
    impl.mNextEventId++;
    auto eventId = impl.mNextEventId;
    mId          = eventId;
}

EventBusImpl& EventBusImpl::getInstance() {
    static std::unique_ptr<EventBusImpl> instance;
    if (!instance) {
        instance = std::make_unique<EventBusImpl>();
    }
    return *instance;
}

EventBus::EventBus() {}

EventBus& EventBus::getInstance() {
    static std::unique_ptr<EventBus> instance;
    if (!instance) {
        instance = std::make_unique<EventBus>();
    }
    return *instance;
}

Listener EventBus::addListener(
    std::type_index             type,
    const std::string&          plugin,
    std::function<void(Event&)> callback,
    uint32_t                    priority
) {
    auto  listener                   = Listener(type, plugin);
    auto& impl                       = EventBusImpl::getInstance();
    impl.mCallbacks[listener]        = std::move(callback);
    impl.mListenerPriority[listener] = priority;
    impl.mListeners[priority].insert(listener);
    return listener;
}

Listener EventBus::addListenerTemp(
    std::type_index             type,
    const std::string&          plugin,
    std::function<void(Event&)> callback,
    uint32_t                    priority,
    std::chrono::milliseconds   duration
) {
    auto listener = addListener(type, plugin, callback, priority);
    ScheduleManager::getInstance().addDelayTask(plugin, duration, [=] { EventBus::removeListener(plugin, listener); });
    return listener;
}

Listener EventBus::addListenerTemp(
    std::type_index             type,
    const std::string&          plugin,
    std::function<void(Event&)> callback,
    uint32_t                    priority,
    size_t                      times
) {
    Listener listener = addListener(
        type,
        plugin,
        [=](Event& ev) {
            callback(ev);
            EventBusImpl::getInstance().mLeftTimes[listener]--;
            if (EventBusImpl::getInstance().mLeftTimes[listener] <= 0) {
                EventBus::removeListener(plugin, listener);
            }
        },
        priority
    );
    EventBusImpl::getInstance().mLeftTimes[listener] = times;
    return listener;
}

Listener EventBus::addListenerTemp(
    std::type_index             type,
    const std::string&          plugin,
    std::function<void(Event&)> callback,
    uint32_t                    priority,
    std::chrono::milliseconds   duration,
    size_t                      times
) {
    Listener listener = addListener(
        type,
        plugin,
        [=](Event& ev) {
            callback(ev);
            EventBusImpl::getInstance().mLeftTimes[listener]--;
            if (EventBusImpl::getInstance().mLeftTimes[listener] <= 0) {
                EventBus::removeListener(plugin, listener);
            }
        },
        priority
    );
    EventBusImpl::getInstance().mLeftTimes[listener] = times;
    ScheduleManager::getInstance().addDelayTask(plugin, duration, [=] { EventBus::removeListener(plugin, listener); });
    return listener;
}

void EventBus::forEachListener(std::type_index type, std::function<bool(const std::function<void(Event&)>&)> func) {
    if (func) {
        auto& impl = EventBusImpl::getInstance();
        for (auto& [priority, listeners] : impl.mListeners) {
            for (auto& listener : listeners) {
                if (listener.mType == type) {
                    if (auto& callback = impl.mCallbacks[listener]) {
                        if (!func(callback)) {
                            return;
                        }
                    }
                }
            }
        }
    }
}

bool EventBus::removeListener(const std::string& plugin, const Listener& listener) {
    if (listener.mPlugin == plugin) {
        auto& impl = EventBusImpl::getInstance();
        if (impl.mCallbacks.contains(listener)) {
            impl.mCallbacks.erase(listener);
            auto priority = impl.mListenerPriority[listener];
            impl.mListeners[priority].erase(listener);
            impl.mListenerPriority.erase(listener);
            impl.mLeftTimes.erase(listener);
            return true;
        }
    }
    return false;
}

void EventBusImpl::removePluginListeners(const std::string& plugin) {
    for (auto& [listener, callback] : mCallbacks) {
        if (listener.mPlugin == plugin) {
            mCallbacks.erase(listener);
            auto priority = mListenerPriority[listener];
            mListeners[priority].erase(listener);
            mListenerPriority.erase(listener);
            mLeftTimes.erase(listener);
        }
    }
}

void EventBusImpl::removeAllListeners() {
    mCallbacks.clear();
    mListeners.clear();
    mListenerPriority.clear();
    mLeftTimes.clear();
}

void EventBus::printException(const std::string& ex) {
    return KobeBryant::getInstance().getLogger().error("bot.catch.exception", {ex});
}