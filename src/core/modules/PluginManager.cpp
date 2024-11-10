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
        instance          = std::make_unique<PluginManager>();
        auto nativeEngine = std::make_shared<NativePluginEngine>();
        instance->mPluginEngines.push_back(nativeEngine);
        instance->mTypesMap["native"] = nativeEngine;
    }
    return *instance;
}

void PluginManager::loadAllPlugins() {
    try {
        auto& logger = KobeBryant::getInstance().getLogger();
        logger.info("bot.plugins.loadingAll");
        int count = 0;
        for (auto& enginePtr : mPluginEngines) {
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

bool PluginManager::loadPlugin(std::string const& name, bool force) {
    try {
        if (!hasPlugin(name)) {
            if (fs::exists("./plugins/" + name + "/manifest.json")) {
                if (auto manifest = PluginManifest::readFrom("./plugins/" + name + "/manifest.json")) {
                    if (name == manifest->mName) {
                        int temp = 0;
                        return loadPlugin(*manifest, manifest->mType, temp, force);
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

bool PluginManager::loadPlugin(PluginManifest const& manifest, std::string const& type, int& count, bool force) {
    try {
        if ((!manifest.mPassive || force)) {
            auto                  type      = manifest.mType;
            auto                  name      = manifest.mName;
            auto&                 logger    = KobeBryant::getInstance().getLogger();
            std::filesystem::path entryPath = "./plugins/" + name + "/" + manifest.mEntry;
            if (fs::exists(entryPath)) {
                for (auto& depe : manifest.mDependence) {
                    if (!hasPlugin(depe)) {
                        if (loadPlugin(depe, true)) {
                            count++;
                            mPluginRely[depe].insert(name);
                        } else {
                            logger.error("bot.plugin.dependenceMiss", {depe, name});
                            return false;
                        }
                    }
                }
                for (auto& opde : manifest.mOptionalDependence) {
                    if (loadPlugin(opde, true)) {
                        count++;
                    }
                }
                if (isValidType(type)) {
                    if (mTypesMap[type]->loadPlugin(name, entryPath)) {
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
            if (force && !mPluginRely[name].empty()) {
                for (auto& rely : mPluginRely[name]) {
                    unloadPlugin(rely, true);
                }
            }
            if (mPluginRely[name].empty()) {
                auto type = mPluginsMap[name];
                if (mTypesMap[type]->unloadPlugin(name)) {
                    mPluginsMap.erase(name);
                    for (auto& [plugin, relys] : mPluginRely) {
                        relys.erase(name);
                    }
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

void PluginManager::loadPluginEngines() {
    try {
        if (!fs::exists("./plugins")) {
            fs::create_directories("./plugins");
        }
        auto  paths  = utils::getAllFileDirectories("./plugins");
        auto& logger = KobeBryant::getInstance().getLogger();
        for (auto& path : paths) {
            if (fs::exists(path / "manifest.json")) {
                if (auto manifest = PluginManifest::readFrom(path / "manifest.json")) {
                    if (manifest->mType == "engine") {
                        auto name = manifest->mName;
                        if (path.filename().string() == name) {
                            auto entry = utils::stringtoWstring("./plugins/" + name + "/" + manifest->mEntry);
                            if (HMODULE hMoudle = LoadLibrary(entry.c_str())) {
                                addModule(hMoudle, name);
                                mEngineHandle[name] = hMoudle;
                                logger.info("bot.pluginEngine.loaded", {name});
                            }
                        } else {
                            logger.error("bot.pluginEngine.nameMismatch", {name, path.filename().string(), name});
                        }
                    }
                }
            }
        }
    }
    CATCH
}

bool PluginManager::registerPluginEngine(std::shared_ptr<IPluginEngine> engine) {
    auto type = engine->getPluginType();
    if (!isValidType(type)) {
        mTypesMap[type] = engine;
        mPluginEngines.push_back(engine);
        KobeBryant::getInstance().getLogger().info("bot.pluginEngine.registered", {engine->getPluginType()});
        return true;
    }
    return false;
}

bool PluginEngineRegistry::registerPluginEngine(std::shared_ptr<IPluginEngine> engine) {
    return PluginManager::getInstance().registerPluginEngine(std::move(engine));
}

void PluginManager::unloadPluginEngines() {
    try {
        mPluginEngines.clear();
        mTypesMap.clear();
        for (auto& [en, handle] : mEngineHandle) {
            FreeLibrary(handle);
        }
        mEngineHandle.clear();
    }
    CATCH
}

void PluginManager::addModule(HMODULE handle, std::string const& name) { mModuleNames[handle] = name; }

void PluginManager::removeModule(HMODULE handle) { mModuleNames.erase(handle); }

std::optional<std::string> PluginManager::getModuleName(HMODULE handle) {
    if (mModuleNames.contains(handle)) {
        return mModuleNames[handle];
    }
    return {};
}