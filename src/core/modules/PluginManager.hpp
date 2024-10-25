#pragma once
#include "core/Global.hpp"

struct PluginManifest {
    std::string mName;
    std::string mEntry;
    bool        mPassive = false;

    static std::optional<PluginManifest> readFrom(std::filesystem::path const& path);
};

class PluginManager {
private:
    std::unordered_map<std::string, HMODULE> mPluginsMap1;
    std::unordered_map<HMODULE, std::string> mPluginsMap2;

public:
    static PluginManager& getInstance();

    void loadAllPlugins();

    void unloadAllPlugins();

    bool loadPlugin(std::string const& folderName);

    bool loadPlugin(std::filesystem::path const& path);

    bool unloadPlugin(std::string const& name);

    bool unloadPlugin(HMODULE hModule);

    std::vector<std::string> getAllPlugins();

    bool hasPlugin(std::string const& name);
};
