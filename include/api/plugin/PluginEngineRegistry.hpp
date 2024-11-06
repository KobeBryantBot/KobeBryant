#pragma once
#include "IPluginEngine.hpp"
#include "Macros.hpp"
#include <concepts>
#include <iostream>

class PluginEngineRegistry {
protected:
    KobeBryant_NDAPI static bool registerPluginEngine(std::shared_ptr<IPluginEngine> engine);

public:
    template <std::derived_from<IPluginEngine> T, typename... Args>
    static inline bool registerPluginEngine(Args... args) {
        auto engine = std::make_shared<T>(std::forward<Args>(args)...);
        return registerPluginEngine(std::move(engine));
    }
};
