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
    HMODULE               mModule;
    KobeBryant_NDAPI      Listener(std::type_index type, HMODULE hModule);
    KobeBryant_NDAPI bool operator<(const Listener& rhs) const;
    KobeBryant_NDAPI bool operator==(const Listener& rhs) const;
};

class EventBus {
protected:
    KobeBryant_API void addListener(Listener const&, std::function<void(Event const&)>);
    KobeBryant_API void forEachListener(std::function<void(Listener const&, std::function<void(Event const&)> const&)>);
    KobeBryant_API void printException(std::string const& ex);

public:
    EventBus();

    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;

    KobeBryant_NDAPI static EventBus& getInstance();

    template <std::derived_from<Event> T>
    inline Listener subscribe(std::function<void(T const&)> callback) {
        auto type     = std::type_index(typeid(T));
        auto hModule  = utils::getCurrentModuleHandle();
        auto listener = Listener(type, hModule);
        addListener(listener, [=](const Event& event) {
            try {
                if (callback) {
                    T const& ev = dynamic_cast<T const&>(event);
                    callback(ev);
                }
            } catch (const std::exception& e) {
                printException(e.what());
            }
        });
        return listener;
    }

    KobeBryant_API bool unsubscribe(Listener const& listener);

    template <std::derived_from<Event> T>
    inline void publish(T const& ev) {
        auto type = std::type_index(typeid(T));
        forEachListener([&](Listener const& listener, std::function<void(Event const&)> const& callback) {
            if (listener.mType == type && callback) {
                callback(ev);
            }
        });
    }
};