#pragma once
#include "api/plugin/IPluginEngine.hpp"
#include "api/plugin/PluginManifest.hpp"
#include "core/Global.hpp"
#include <unordered_set>

class NativePluginEngine : public IPluginEngine {
private:
    std::unordered_map<std::string, HMODULE> mPluginsMap1;
    std::unordered_map<HMODULE, std::string> mPluginsMap2;

public:
    std::string getPluginType() const override;

    bool loadPlugin(std::string const& name, std::filesystem::path const& entry) override;

    bool unloadPlugin(std::string const& name) override;

    bool unloadPlugin(HMODULE hModule);

    HMODULE getPluginHandle(std::string const& name);
};
