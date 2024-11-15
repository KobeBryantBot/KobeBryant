#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <Windows.h>
#include <functional>
#include <iostream>

class CommandRegistry {
public:
    CommandRegistry();

    CommandRegistry(const CommandRegistry&)            = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;

protected:
    KobeBryant_NDAPI bool
    registerSimpleCommand(std::string const&, std::string const&, std::function<void(std::vector<std::string> const&)>);
    KobeBryant_NDAPI bool unregisterCommand(std::string const&, std::string const&);

public:
    KobeBryant_NDAPI static CommandRegistry& getInstance();

    inline bool
    registerSimpleCommand(std::string const& cmd, std::function<void(std::vector<std::string> const&)> callback) {
        return registerSimpleCommand(utils::getCurrentPluginName(), cmd, callback);
    }

    inline bool unregisterCommand(std::string const& cmd) {
        return unregisterCommand(utils::getCurrentPluginName(), cmd);
    }

    KobeBryant_API void executeCommand(std::string const& command);
};
