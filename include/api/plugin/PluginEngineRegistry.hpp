#pragma once
#include "IPluginEngine.hpp"
#include "Macros.hpp"
#include <concepts>
#include <iostream>

class PluginEngineRegistry {
protected:
    static bool registerPluginEngine(std::unique_ptr<IPluginEngine> engine);

public:
    template <std::derived_from<IPluginEngine> T, typename... Args>
    static inline bool registerPluginEngine(Args... args) {
        auto engine = std::make_unique<T>(std::forward<Args>(args)...);
        return registerPluginEngine(std::move(engine));
    }
};
