#pragma once
#include "api/utils/UUID.hpp"
#include "core/Global.hpp"
#include <mutex>

class EventDispatcher {
private:
    std::unordered_map<utils::UUID, std::function<void(nlohmann::json const&)>> mCallbacks;
    std::mutex                                                                  mMutex;

public:
    static EventDispatcher& getInstance();

    void init();

    void addCallback(
        utils::UUID const&                                uuid,
        std::function<void(nlohmann::json const&)> const& callback,
        std::function<void()>                             timeoutCallback,
        uint64_t                                          seconds
    );
};
