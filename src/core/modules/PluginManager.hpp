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

    bool hasPlugin(const std::string& name) const;

    bool isValidType(const std::string& type) const;

    bool registerPluginEngine(const std::string& handle, std::shared_ptr<IPluginEngine> engine);

    void tryRemovePluginEngine(const std::string& handle);

    void loadAllPlugins();

    void loadAllPlugins(std::weak_ptr<IPluginEngine> engine, int& count);

    bool loadPlugin(const std::string& name, bool force = false);

    bool loadDependence(
        const std::string&     name,
        std::optional<Version> minVersion,
        std::optional<Version> maxVersion,
        const std::string&     plugin
    );

    bool loadPlugin(const PluginManifest& manifest, const std::string& type, int& count, bool force = false);

    void unloadAllPlugins();

    bool unloadPlugin(const std::string& name, bool force = false);

    std::vector<std::string> getAllPlugins();

    NativePluginEngine& getNativePluginEngine();
};
