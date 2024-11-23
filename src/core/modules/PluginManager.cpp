#include "PluginManager.hpp"
#include "CommandManager.hpp"
#include "EventBusImpl.hpp"
#include "KobeBryant.hpp"
#include "NativePluginEngine.hpp"
#include "ScheduleManager.hpp"
#include "ServiceManager.hpp"
#include "api/plugin/PluginEngineRegistry.hpp"
#include "api/utils/StringUtils.hpp"

namespace fs = std::filesystem;

PluginManager& PluginManager::getInstance() {
    static std::unique_ptr<PluginManager> instance;
    if (!instance) {
        instance                      = std::make_unique<PluginManager>();
        auto nativeEngine             = std::make_shared<NativePluginEngine>();
        instance->mTypesMap["native"] = nativeEngine;
    }
    return *instance;
}

void PluginManager::loadAllPlugins() {
    try {
        if (!fs::exists("./plugins")) {
            fs::create_directories("./plugins");
        }
        auto& logger = KobeBryant::getInstance().getLogger();
        logger.info("bot.plugins.loadingAll");
        int count = 0;
        loadAllPlugins(mTypesMap["native"], count);
        for (auto& enginePtr : mExtraEngines) {
            auto engine = std::weak_ptr<IPluginEngine>(enginePtr);
            loadAllPlugins(engine, count);
        }
        logger.info("bot.plugins.loadedAll", {S(count)});
    }
    CATCH
}

void PluginManager::loadAllPlugins(std::weak_ptr<IPluginEngine> engine, int& count) {
    try {
        auto  paths  = utils::getAllFileDirectories("./plugins");
        auto  type   = engine.lock()->getPluginType();
        auto& logger = KobeBryant::getInstance().getLogger();
        for (auto& path : paths) {
            if (fs::exists(path / "manifest.json")) {
                if (auto manifest = PluginManifest::readFrom(path / "manifest.json")) {
                    if (manifest->mType == type) {
                        auto name = manifest->mName;
                        if (!hasPlugin(name)) {
                            if (path.filename().string() == name) {
                                if (loadPlugin(manifest.value(), type, count)) {
                                    count++;
                                }
                            } else {
                                logger.error("bot.plugin.nameMismatch", {name, path.filename().string(), name});
                            }
                        }
                    }
                }
            }
        }
    }
    CATCH
}

bool PluginManager::loadPlugin(const std::string& name, bool force) {
    try {
        if (!hasPlugin(name)) {
            if (fs::exists("./plugins/" + name + "/manifest.json")) {
                if (auto manifest = PluginManifest::readFrom("./plugins/" + name + "/manifest.json")) {
                    if (name == manifest->mName) {
                        int temp = 0;
                        if (!manifest->mPassive) {
                            return loadPlugin(*manifest, manifest->mType, temp);
                        }
                    } else {
                        KobeBryant::getInstance().getLogger().error(
                            "bot.plugin.nameMismatch",
                            {manifest->mName, name, manifest->mName}
                        );
                    }
                }
            }
        }
    }
    CATCH
    return false;
}

bool PluginManager::loadDependence(
    const std::string&     name,
    std::optional<Version> minVersion,
    std::optional<Version> maxVersion,
    const std::string&     plugin
) {
    try {
        if (!hasPlugin(name)) {
            if (fs::exists("./plugins/" + name + "/manifest.json")) {
                if (auto manifest = PluginManifest::readFrom("./plugins/" + name + "/manifest.json")) {
                    if (name == manifest->mName) {
                        int temp = 0;
                        if (!((minVersion && manifest->mVersion < *minVersion)
                              || (maxVersion && manifest->mVersion > *maxVersion))) {
                            return loadPlugin(*manifest, manifest->mType, temp, true);
                        }
                        KobeBryant::getInstance().getLogger().error(
                            "plugin.dependence.versionMismatch",
                            {name, plugin}
                        );
                    } else {
                        KobeBryant::getInstance().getLogger().error(
                            "bot.plugin.nameMismatch",
                            {manifest->mName, name, manifest->mName}
                        );
                    }
                }
            }
        }
    }
    CATCH
    return false;
}

bool PluginManager::loadPlugin(const PluginManifest& manifest, const std::string& type, int& count, bool force) {
    try {
        if (!manifest.mPassive || force) {
            auto                  type      = manifest.mType;
            auto                  name      = manifest.mName;
            auto&                 logger    = KobeBryant::getInstance().getLogger();
            std::filesystem::path entryPath = "./plugins/" + name + "/" + manifest.mEntry;
            if (fs::exists(entryPath)) {
                for (auto& depe : manifest.mDependence) {
                    if (!hasPlugin(depe.mName)) {
                        if (loadDependence(depe.mName, depe.mMinVersion, depe.mMaxVersion, name)) {
                            count++;
                            mPluginRely[depe.mName].insert(name);
                        } else {
                            logger.error("bot.plugin.dependenceMiss", {depe.mName, name});
                            return false;
                        }
                    }
                }
                for (auto& opde : manifest.mOptionalDependence) {
                    if (loadDependence(opde.mName, opde.mMinVersion, opde.mMaxVersion, name)) {
                        count++;
                    }
                }
                for (auto& preload : manifest.mPreload) {
                    std::filesystem::path preloadPath(preload);
                    if (!fs::exists(preloadPath)) {
                        preloadPath  = std::filesystem::path("./plugins/" + name);
                        preloadPath /= preload;
                    }
                    if (auto handle = LoadLibrary(preloadPath.wstring().c_str())) {
                        mPluginPreload[name].insert(handle);
                    } else {
                        logger.error("plugin.preload.miss", {name, preload});
                        return false;
                    }
                }
                if (isValidType(type)) {
                    if (mTypesMap[type]->loadPlugin(name, entryPath)) {
                        auto relyName = mTypesName[type];
                        mPluginRely[relyName].insert(name);
                        mPluginsMap[name] = type;
                        return true;
                    } else {
                        logger.error("bot.plugin.load.fail", {name});
                    }
                }
            } else {
                logger.error("bot.plugin.noEntry", {name, manifest.mEntry});
            }
        }
    }
    CATCH
    return false;
}

bool PluginManager::hasPlugin(const std::string& name) const { return mPluginsMap.contains(name); }

bool PluginManager::isValidType(const std::string& name) const { return mTypesMap.contains(name); }

void PluginManager::unloadAllPlugins() {
    try {
        EventBusImpl::getInstance().removeAllListeners();
        CommandManager::getInstance().unregisterAllCommands();
        ScheduleManager::getInstance().removeAllTasks();
        ServiceManager::getInstance().removeAllFunc();
        auto& logger = KobeBryant::getInstance().getLogger();
        logger.info("bot.plugins.unloadingAll");
        for (auto& [name, type] : mPluginsMap) {
            if (type != "native") {
                unloadPlugin(name, true);
            }
        }
        for (auto& [name, type] : mPluginsMap) {
            unloadPlugin(name, true);
        }
        mPluginsMap.clear();
        mPluginPreload.clear();
        logger.info("bot.plugins.unloadedAll");
    } catch (...) {}
}

bool PluginManager::unloadPlugin(const std::string& name, bool force) {
    try {
        auto& logger = KobeBryant::getInstance().getLogger();
        if (hasPlugin(name)) {
            if (force) {
                for (auto& rely : mPluginRely[name]) {
                    if (unloadPlugin(rely, true)) {
                        mPluginRely[name].erase(rely);
                    }
                }
            }
            if (mPluginRely[name].empty()) {
                auto type = mPluginsMap[name];
                ServiceManager::getInstance().removePluginFunc(name);
                EventBusImpl::getInstance().removePluginListeners(name);
                CommandManager::getInstance().unregisterPluginCommands(name);
                ScheduleManager::getInstance().removePluginTasks(name);
                if (mTypesMap[type]->unloadPlugin(name)) {
                    auto name_ = name;
                    for (auto& [plugin, relys] : mPluginRely) {
                        mPluginRely[plugin].erase(name_);
                    }
                    mPluginsMap.erase(name_);
                    for (auto& preload : mPluginPreload[name_]) {
                        FreeLibrary(preload);
                    }
                    mPluginPreload.erase(name_);
                    return true;
                } else {
                    logger.error("bot.plugin.unload.error", {name});
                }
            } else {
                std::string relylist;
                for (auto& rely : mPluginRely[name]) {
                    relylist.append(rely + ", ");
                }
                relylist.erase(relylist.size() - 2);
                logger.error("bot.plugin.unload.dependence", {name, relylist});
            }
        }
    }
    CATCH
    return false;
}

std::vector<std::string> PluginManager::getAllPlugins() {
    std::vector<std::string> result;
    for (auto& [name, type] : mPluginsMap) {
        result.push_back(name);
    }
    return result;
}

NativePluginEngine& PluginManager::getNativePluginEngine() {
    return *static_cast<NativePluginEngine*>(mTypesMap["native"].get());
}

bool PluginManager::registerPluginEngine(const std::string& handle, std::shared_ptr<IPluginEngine> engine) {
    auto type = engine->getPluginType();
    if (!isValidType(type)) {
        mTypesName[type]     = handle;
        mHandleTypes[handle] = type;
        mTypesMap[type]      = engine;
        mExtraEngines.push_back(engine);
        KobeBryant::getInstance().getLogger().info("bot.pluginEngine.registered", {engine->getPluginType()});
        return true;
    }
    return false;
}

void PluginManager::tryRemovePluginEngine(const std::string& handle) {
    if (mHandleTypes.contains(handle)) {
        auto type = mHandleTypes[handle];
        mTypesMap.erase(type);
        mTypesName.erase(type);
        mExtraEngines.erase(
            std::remove_if(
                mExtraEngines.begin(),
                mExtraEngines.end(),
                [type](std::shared_ptr<IPluginEngine> engine) -> bool { return engine->getPluginType() == type; }
            ),
            mExtraEngines.end()
        );
        mHandleTypes.erase(handle);
    }
}

bool PluginEngineRegistry::registerPluginEngine(const std::string& handle, std::shared_ptr<IPluginEngine> engine) {
    return PluginManager::getInstance().registerPluginEngine(handle, std::move(engine));
}