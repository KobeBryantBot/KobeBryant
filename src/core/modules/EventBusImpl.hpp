#include "api/EventBus.hpp"
#include "core/Global.hpp"
#include <set>

class EventBusImpl {
public:
    std::map<Listener, std::function<void(Event&)>> mCallbacks;
    std::map<uint32_t, std::set<Listener>>          mListeners;
    std::map<Listener, uint32_t>                    mListenerPriority;
    uint64_t                                        mNextEventId = 0;

    static EventBusImpl& getInstance();

    void removePluginListeners(const std::string& plugin);

    void removeAllListeners();
};