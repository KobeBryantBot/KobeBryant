#include "api/EventBus.hpp"
#include "core/Global.hpp"

class EventBusImpl {
public:
    std::map<Listener, std::function<void(Event const&)>> mCallbacks;
    uint64_t                                              mNextEventId = 0;

    static EventBusImpl& getInstance();

    void removePluginListeners(HMODULE hModule);

    void removeAllListeners();
};