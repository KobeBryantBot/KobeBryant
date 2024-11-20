#include "KobeBryant.hpp"
#include "api/Logger.hpp"
#include "core/modules/EventBusImpl.hpp"

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

void EventBus::addListener(const Listener& listener, std::function<void(Event&)> callback, uint32_t priority) {
    auto& impl                       = EventBusImpl::getInstance();
    impl.mCallbacks[listener]        = std::move(callback);
    impl.mListenerPriority[listener] = priority;
    impl.mListeners[priority].insert(listener);
}

void EventBus::forEachListener(std::type_index type, std::function<bool(std::function<void(Event&)> const&)> func) {
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
        }
    }
}

void EventBusImpl::removeAllListeners() { mCallbacks.clear(); }

void EventBus::printException(const std::string& ex) {
    return KobeBryant::getInstance().getLogger().error("bot.catch.exception", {ex});
}