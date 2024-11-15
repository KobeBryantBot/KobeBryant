#pragma once
#include "IPluginEngine.hpp"
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <concepts>
#include <iostream>

class PluginEngineRegistry {
protected:
    KobeBryant_NDAPI static bool registerPluginEngine(std::string const&, std::shared_ptr<IPluginEngine>);

public:
    template <std::derived_from<IPluginEngine> T, typename... Args>
    static inline bool registerPluginEngine(Args... args) {
        auto engine = std::make_shared<T>(std::forward<Args>(args)...);
        auto handle = utils::getCurrentPluginName();
        return registerPluginEngine(handle, std::move(engine));
    }
};
