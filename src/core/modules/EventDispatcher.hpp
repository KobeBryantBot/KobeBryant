#pragma once
#include "api/utils/UUID.hpp"
#include "core/Global.hpp"
#include <mutex>

class EventDispatcher {
private:
    std::unordered_map<utils::UUID, std::function<void(const nlohmann::json&)>> mCallbacks;
    std::mutex                                                                  mMutex;

public:
    static EventDispatcher& getInstance();

    void init();

    void addCallback(
        const utils::UUID&                                uuid,
        std::function<void(const nlohmann::json&)> const& callback,
        std::function<void()>                             timeoutCallback,
        uint64_t                                          seconds
    );
};
