#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <functional>
#include <iostream>

// TODO: 完全重写此类
class CommandRegistry {
public:
    CommandRegistry();

    CommandRegistry(const CommandRegistry&)            = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;

protected:
    KobeBryant_NDAPI bool
    registerSimpleCommand(const std::string&, const std::string&, std::function<void(const std::vector<std::string>&)>);
    KobeBryant_NDAPI bool unregisterCommand(const std::string&, const std::string&);

public:
    KobeBryant_NDAPI static CommandRegistry& getInstance();

    inline bool
    registerSimpleCommand(const std::string& cmd, std::function<void(const std::vector<std::string>&)> callback) {
        return registerSimpleCommand(utils::getCurrentPluginName(), cmd, callback);
    }

    inline bool unregisterCommand(const std::string& cmd) {
        return unregisterCommand(utils::getCurrentPluginName(), cmd);
    }

    KobeBryant_API void executeCommand(const std::string& command);
};
