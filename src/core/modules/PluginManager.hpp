#pragma once
#include "NativePluginEngine.hpp"
#include "api/plugin/IPluginEngine.hpp"
#include "api/plugin/PluginManifest.hpp"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

class PluginManager {
private:
    std::unordered_map<std::string, std::string>                     mPluginsMap;
    std::unordered_map<std::string, std::shared_ptr<IPluginEngine>>  mTypesMap;
    std::vector<std::shared_ptr<IPluginEngine>>                      mPluginEngines;
    std::unordered_map<std::string, std::unordered_set<std::string>> mPluginRely;

public:
    static PluginManager& getInstance();

    bool hasPlugin(std::string const& name) const;

    bool isValidType(std::string const& type) const;

    void loadPluginEngines();

    void loadAllPlugins();

    void loadAllPlugins(std::weak_ptr<IPluginEngine> engine, int& count);

    bool loadPlugin(std::string const& name, bool force = false);

    bool loadPlugin(PluginManifest const& manifest, std::string const& type, int& count, bool force = false);

    void unloadAllPlugins();

    bool unloadPlugin(std::string const& name, bool force = false);

    std::vector<std::string> getAllPlugins();

    NativePluginEngine& getNativePluginEngine();
};
