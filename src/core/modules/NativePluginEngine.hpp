#pragma once
#include "PluginManifest.hpp"
#include "api/plugin/IPluginEngine.hpp"
#include "core/Global.hpp"
#include <unordered_set>

class NativePluginEngine : public IPluginEngine {
private:
    std::unordered_map<std::string, HMODULE> mPluginsMap1;
    std::unordered_map<HMODULE, std::string> mPluginsMap2;

public:
    std::string getPluginType() const override;

    bool loadPlugin(const std::string& name, const std::filesystem::path& entry) override;

    bool unloadPlugin(const std::string& name) override;

    bool unloadPlugin(HMODULE hModule);

    HMODULE getPluginHandle(const std::string& name);
};
