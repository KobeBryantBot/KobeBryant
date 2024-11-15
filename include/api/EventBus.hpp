#pragma once
#include "Macros.hpp"
#include "api/event/Event.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <Windows.h>
#include <concepts>
#include <functional>
#include <iostream>
#include <map>
#include <typeindex>

struct Listener {
    uint64_t              mId;
    std::type_index       mType;
    std::string           mPlugin;
    KobeBryant_NDAPI      Listener(std::type_index type, std::string const& plugin);
    KobeBryant_NDAPI bool operator<(const Listener& rhs) const;
    KobeBryant_NDAPI bool operator==(const Listener& rhs) const;
};

class EventBus {
protected:
    KobeBryant_API void addListener(Listener const&, std::function<void(Event&)>, uint32_t);
    KobeBryant_API void forEachListener(std::type_index, std::function<bool(std::function<void(Event&)> const&)>);
    KobeBryant_API bool removeListener(std::string const&, Listener const&);
    KobeBryant_API void printException(std::string const&);

public:
    EventBus();

    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;

    KobeBryant_NDAPI static EventBus& getInstance();

    template <std::derived_from<Event> T>
    inline Listener subscribe(std::function<void(T&)> callback, uint32_t priority = 500) {
        auto type     = std::type_index(typeid(T));
        auto plugin   = utils::getCurrentPluginName();
        auto listener = Listener(type, plugin);
        addListener(
            listener,
            [=](Event& event) {
                try {
                    if (callback) {
                        T& ev = dynamic_cast<T&>(event);
                        callback(ev);
                    }
                } catch (const std::exception& e) {
                    printException(e.what());
                }
            },
            priority
        );
        return listener;
    }

    inline bool unsubscribe(Listener const& listener) {
        auto plugin = utils::getCurrentPluginName();
        return removeListener(plugin, listener);
    }

    template <std::derived_from<Event> T>
    inline void publish(T& ev) {
        auto type = std::type_index(typeid(T));
        forEachListener(type, [&](std::function<void(Event&)> const& callback) -> bool {
            callback(ev);
            return !ev.isPassingBlocked();
        });
    }
};