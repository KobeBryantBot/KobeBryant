#include "api/Logger.hpp"
#include "core/modules/EventBusImpl.hpp"

bool Listener::operator<(const Listener& rhs) const { return mId < rhs.mId; }

bool Listener::operator==(const Listener& rhs) const { return mId == rhs.mId; }

Listener::Listener(std::type_index type, HMODULE hModule) : mType(type), mModule(hModule) {
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

void EventBus::addListener(Listener const& listener, std::function<void(Event const&)> callback) {
    auto& impl                = EventBusImpl::getInstance();
    impl.mCallbacks[listener] = std::move(callback);
}

void EventBus::forEachListener(std::function<void(Listener const&, std::function<void(Event const&)> const&)> func) {
    if (func) {
        auto& impl = EventBusImpl::getInstance();
        for (auto& [listener, callback] : impl.mCallbacks) {
            func(listener, callback);
        }
    }
}

bool EventBus::unsubscribe(Listener const& listener) {
    auto& impl = EventBusImpl::getInstance();
    if (impl.mCallbacks.contains(listener)) {
        impl.mCallbacks.erase(listener);
        return true;
    }
    return false;
}

void EventBusImpl::removePluginListeners(HMODULE hModule) {
    for (auto& [listener, callback] : mCallbacks) {
        if (listener.mModule == hModule) {
            mCallbacks.erase(listener);
        }
    }
}

void EventBusImpl::removeAllListeners() { mCallbacks.clear(); }