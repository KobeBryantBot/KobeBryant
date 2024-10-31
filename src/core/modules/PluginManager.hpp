#pragma once
#include "core/Global.hpp"
#include <unordered_set>

struct PluginManifest {
    std::string              mName;
    std::string              mEntry;
    std::string              mType               = "native";
    bool                     mPassive            = false;
    std::vector<std::string> mDependence         = {};
    std::vector<std::string> mOptionalDependence = {};

    static std::optional<PluginManifest> readFrom(std::filesystem::path const& path);
};

class PluginManager {
private:
    std::unordered_map<std::string, HMODULE>                         mPluginsMap1;
    std::unordered_map<HMODULE, std::string>                         mPluginsMap2;
    std::unordered_map<std::string, std::unordered_set<std::string>> mPluginRely;

public:
    static PluginManager& getInstance();

    void loadAllPlugins();

    void unloadAllPlugins();

    bool loadPlugin(std::string const& folderName, bool forceLoad = false);

    bool loadPlugin(std::filesystem::path const& path, int& count, bool forceLoad = false);

    bool unloadPlugin(std::string const& name, bool force = false);

    bool unloadPlugin(HMODULE hModule, bool force = false);

    std::vector<std::string> getAllPlugins();

    bool hasPlugin(std::string const& name);

    HMODULE getPluginHandle(std::string const& name);
};
