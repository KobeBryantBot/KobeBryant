#pragma once
#include "Macros.hpp"
#include "api/event/Event.hpp"
#include "api/utils/ModuleUtils.hpp"
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
    EventBus();

    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;

    KobeBryant_NDAPI static EventBus& getInstance();

    template <std::derived_from<Event> E>
    inline Listener subscribe(std::function<void(E&)> callback, uint32_t priority = 500) {
        return addListener(std::type_index(typeid(E)), utils::getCurrentPluginName(), EVENT_CALLBACK, priority);
    }

    template <std::derived_from<Event> E, class T, class D>
    inline Listener
    subscribeTemp(std::chrono::duration<T, D> duration, std::function<void(E&)> callback, uint32_t priority = 500) {
        return addListenerTemp(
            std::type_index(typeid(E)),
            utils::getCurrentPluginName(),
            EVENT_CALLBACK,
            priority,
            std::chrono::duration_cast<std::chrono::milliseconds>(duration)
        );
    }

    template <std::derived_from<Event> E>
    inline Listener subscribeTemp(size_t times, std::function<void(E&)> callback, uint32_t priority = 500) {
        return addListenerTemp(
            std::type_index(typeid(E)),
            utils::getCurrentPluginName(),
            EVENT_CALLBACK,
            priority,
            times
        );
    }

    template <std::derived_from<Event> E, class T, class D>
    inline Listener subscribeTemp(
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

    inline bool unsubscribe(const Listener& listener) {
        return removeListener(utils::getCurrentPluginName(), listener);
    }

    template <std::derived_from<Event> T>
    inline void publish(T& ev) {
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
    KobeBryant_NDAPI Listener addListener(std::type_index, const std::string&, std::function<void(Event&)>, uint32_t);
    KobeBryant_NDAPI Listener addListenerTemp(
        std::type_index,
        const std::string&,
        std::function<void(Event&)>,
        uint32_t,
        std::chrono::milliseconds
    );
    KobeBryant_NDAPI Listener
    addListenerTemp(std::type_index, const std::string&, std::function<void(Event&)>, uint32_t, size_t);
    KobeBryant_NDAPI Listener addListenerTemp(
        std::type_index,
        const std::string&,
        std::function<void(Event&)>,
        uint32_t,
        std::chrono::milliseconds,
        size_t
    );
    KobeBryant_API void forEachListener(std::type_index, std::function<bool(const std::function<void(Event&)>&)>);
    KobeBryant_API bool removeListener(const std::string&, const Listener&);
    KobeBryant_API void printException(const std::string&);
};