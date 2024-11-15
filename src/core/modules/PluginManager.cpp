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

bool PluginManager::loadPlugin(std::string const& name, bool isDependence) {
    try {
        if (!hasPlugin(name)) {
            if (fs::exists("./plugins/" + name + "/manifest.json")) {
                if (auto manifest = PluginManifest::readFrom("./plugins/" + name + "/manifest.json")) {
                    if (name == manifest->mName) {
                        int temp = 0;
                        if (!manifest->mPassive || isDependence) {
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

bool PluginManager::loadPlugin(PluginManifest const& manifest, std::string const& type, int& count) {
    try {
        auto                  type      = manifest.mType;
        auto                  name      = manifest.mName;
        auto&                 logger    = KobeBryant::getInstance().getLogger();
        std::filesystem::path entryPath = "./plugins/" + name + "/" + manifest.mEntry;
        if (fs::exists(entryPath)) {
            for (auto& depe : manifest.mDependence) {
                if (!hasPlugin(depe.mName)) {
                    if (loadPlugin(depe.mName, true)) {
                        count++;
                        mPluginRely[depe.mName].insert(name);
                    } else {
                        logger.error("bot.plugin.dependenceMiss", {depe.mName, name});
                        return false;
                    }
                }
            }
            for (auto& opde : manifest.mOptionalDependence) {
                if (loadPlugin(opde.mName, true)) {
                    count++;
                }
            }
            for (auto& preload : manifest.mPreload) {
                std::filesystem::path preloadPath("./plugins/" + name);
                preloadPath /= preload;
                if (!LoadLibrary(preloadPath.wstring().c_str())) {
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
    CATCH
    return false;
}

bool PluginManager::hasPlugin(std::string const& name) const { return mPluginsMap.contains(name); }

bool PluginManager::isValidType(std::string const& name) const { return mTypesMap.contains(name); }

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
        logger.info("bot.plugins.unloadedAll");
    } catch (...) {}
}

bool PluginManager::unloadPlugin(std::string const& name, bool force) {
    try {
        auto& logger = KobeBryant::getInstance().getLogger();
        if (hasPlugin(name)) {
            ServiceManager::getInstance().removePluginFunc(name);
            if (force) {
                for (auto& rely : mPluginRely[name]) {
                    if (unloadPlugin(rely, true)) {
                        mPluginRely[name].erase(rely);
                    }
                }
            }
            if (mPluginRely[name].empty()) {
                auto type = mPluginsMap[name];
                if (mTypesMap[type]->unloadPlugin(name)) {
                    auto name_ = name;
                    for (auto& [plugin, relys] : mPluginRely) {
                        mPluginRely[plugin].erase(name_);
                    }
                    mPluginsMap.erase(name_);
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

bool PluginManager::registerPluginEngine(HMODULE handle, std::shared_ptr<IPluginEngine> engine) {
    auto type = engine->getPluginType();
    if (!isValidType(type)) {
        auto name            = utils::getPluginModuleName(handle);
        mTypesName[type]     = name;
        mHandleTypes[handle] = type;
        mTypesMap[type]      = engine;
        mExtraEngines.push_back(engine);
        KobeBryant::getInstance().getLogger().info("bot.pluginEngine.registered", {engine->getPluginType()});
        return true;
    }
    return false;
}

void PluginManager::tryRemovePluginEngine(HMODULE handle) {
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

bool PluginEngineRegistry::registerPluginEngine(HMODULE handle, std::shared_ptr<IPluginEngine> engine) {
    return PluginManager::getInstance().registerPluginEngine(handle, std::move(engine));
}