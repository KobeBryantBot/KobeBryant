#pragma once
#include "NativePluginEngine.hpp"
#include "PluginManifest.hpp"
#include "api/plugin/IPluginEngine.hpp"
#include <iostream>
#include <unordered_map>
#include <unordered_set>

class PluginManager {
private:
    std::unordered_map<std::string, std::string>                     mPluginsMap;
    std::unordered_map<std::string, std::shared_ptr<IPluginEngine>>  mTypesMap;
    std::unordered_map<std::string, std::string>                     mTypesName;
    std::unordered_map<std::string, std::string>                     mHandleTypes;
    std::vector<std::shared_ptr<IPluginEngine>>                      mExtraEngines;
    std::unordered_map<std::string, std::unordered_set<std::string>> mPluginRely;

public:
    static PluginManager& getInstance();

    bool hasPlugin(std::string const& name) const;

    bool isValidType(std::string const& type) const;

    bool registerPluginEngine(std::string const& handle, std::shared_ptr<IPluginEngine> engine);

    void tryRemovePluginEngine(std::string const& handle);

    void loadAllPlugins();

    void loadAllPlugins(std::weak_ptr<IPluginEngine> engine, int& count);

    bool loadPlugin(std::string const& name, bool force = false);

    bool loadDependence(
        std::string const&     name,
        std::optional<Version> minVersion,
        std::optional<Version> maxVersion,
        std::string const&     plugin
    );

    bool loadPlugin(PluginManifest const& manifest, std::string const& type, int& count, bool force = false);

    void unloadAllPlugins();

    bool unloadPlugin(std::string const& name, bool force = false);

    std::vector<std::string> getAllPlugins();

    NativePluginEngine& getNativePluginEngine();
};
