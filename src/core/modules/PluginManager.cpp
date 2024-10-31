#include "core/modules/PluginManager.hpp"
#include "CommandManager.hpp"
#include "KobeBryant.hpp"
#include "ScheduleManager.hpp"
#include "ServiceManager.hpp"
#include "core/Global.hpp"
#include "core/modules/EventBusImpl.hpp"

namespace fs = std::filesystem;

std::optional<std::wstring> getDllPath(std::string const str) {
    int numChars = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    if (numChars > 0) {
        std::vector<wchar_t> wideChars(numChars);
        int                  numConverted = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wideChars[0], numChars);
        if (numConverted > 0) {
            std::wstring wideString(wideChars.begin(), wideChars.end() - 1);
            return wideString;
        }
    }
    return {};
}

std::optional<PluginManifest> PluginManifest::readFrom(std::filesystem::path const& path) {
    try {
        if (auto file = utils::readFile(path)) {
            auto data = nlohmann::json::parse(*file, nullptr, true, true);
            if (data.contains("name") && data.contains("entry")) {
                PluginManifest result;
                result.mName  = data["name"];
                result.mEntry = data["entry"];
                if (data.contains("passive")) {
                    result.mPassive = data["passive"];
                }
                if (data.contains("dependence")) {
                    result.mDependence = data["dependence"];
                }
                if (data.contains("optional_dependence")) {
                    result.mOptionalDependence = data["optional_dependence"];
                }
                return result;
            }
        }
    }
    CATCH
    return {};
}

PluginManager& PluginManager::getInstance() {
    static std::unique_ptr<PluginManager> instance;
    if (!instance) {
        instance = std::make_unique<PluginManager>();
    }
    return *instance;
}

void PluginManager::loadAllPlugins() {
    if (!fs::exists("./plugins")) {
        fs::create_directories("./plugins");
    }
    KobeBryant::getInstance().getLogger().info("bot.plugins.loadingAll");
    int  count = 0;
    auto paths = utils::getAllFileDirectories("./plugins");
    for (auto& path : paths) {
        if (loadPlugin(path, count)) {
            count++;
        }
    }
    KobeBryant::getInstance().getLogger().info("bot.plugins.loadedAll", {S(count)});
}

std::string getErrorReason(unsigned long errorCode) {
    if (errorCode == 126) {
        return tr("bot.errorCode.126");
    } else if (errorCode == 127) {
        return tr("bot.errorCode.127");
    }
    return {};
}

bool PluginManager::loadPlugin(std::filesystem::path const& path, int& count, bool forceLoad) {
    try {
        auto& logger = KobeBryant::getInstance().getLogger();
        if (fs::exists(path / "manifest.json")) {
            if (auto manifest = PluginManifest::readFrom(path / "manifest.json")) {
                if (!manifest->mPassive || forceLoad) {
                    auto name = manifest->mName;
                    if (fs::exists(path / manifest->mEntry)) {
                        if (path.filename().string() == name) {
                            if (auto entry = getDllPath(path.string() + "/" + manifest->mEntry)) {
                                for (auto& depe : manifest->mDependence) {
                                    if (loadPlugin(depe, true)) {
                                        count++;
                                        mPluginRely[name].insert(depe);
                                    } else {
                                        logger.error("bot.plugin.dependenceMiss", {depe, name});
                                        return false;
                                    }
                                }
                                for (auto& opde : manifest->mOptionalDependence) {
                                    if (loadPlugin(opde, true)) {
                                        count++;
                                    }
                                }
                                if (!hasPlugin(name)) {
                                    if (HMODULE hMoudle = LoadLibrary(entry->c_str())) {
                                        mPluginsMap1[name]    = hMoudle;
                                        mPluginsMap2[hMoudle] = name;
                                        logger.info("bot.plugin.loaded", {name});
                                        return true;
                                    } else {
                                        DWORD errorCode = GetLastError();
                                        auto  reason    = getErrorReason(errorCode);
                                        logger.error("bot.plugin.load.fail", {name, S(errorCode), reason});
                                    }
                                }
                            }
                        } else {
                            logger.error("bot.plugin.nameMismatch", {name, path.filename().string(), name});
                        }
                    } else {
                        logger.error("bot.plugin.noEntry", {name, manifest->mEntry});
                    }
                }
            }
        }
    }
    CATCH
    return false;
}

bool PluginManager::loadPlugin(std::string const& folderName, bool forceLoad) {
    auto path  = std::filesystem::path("./plugins");
    path      /= folderName;
    int count  = 0;
    return loadPlugin(path, count, forceLoad);
}

void PluginManager::unloadAllPlugins() {
    auto& logger = KobeBryant::getInstance().getLogger();
    logger.info("bot.plugins.unloadingAll");
    for (auto& [name, hMoudle] : mPluginsMap1) {
        unloadPlugin(hMoudle, true);
    }
    EventBusImpl::getInstance().removeAllListeners();
    CommandManager::getInstance().unregisterAllCommands();
    ScheduleManager::getInstance().removeAllTasks();
    ServiceManager::getInstance().removeAllFunc();
    mPluginsMap1.clear();
    mPluginsMap2.clear();
    logger.info("bot.plugins.unloadedAll");
}

bool PluginManager::unloadPlugin(std::string const& name, bool force) {
    if (mPluginsMap1.contains(name)) {
        auto hModule = mPluginsMap1[name];
        return unloadPlugin(hModule, force);
    }
    return false;
}

bool PluginManager::unloadPlugin(HMODULE hModule, bool force) {
    try {
        auto& logger = KobeBryant::getInstance().getLogger();
        if (mPluginsMap2.contains(hModule)) {
            auto name = mPluginsMap2[hModule];
            if (force && !mPluginRely.empty()) {
                for (auto& rely : mPluginRely[name]) {
                    unloadPlugin(rely, true);
                }
            }
            if (mPluginRely[name].empty()) {
                EventBusImpl::getInstance().removePluginListeners(hModule);
                CommandManager::getInstance().unregisterPluginCommands(hModule);
                ScheduleManager::getInstance().removePluginTasks(hModule);
                ServiceManager::getInstance().removePluginFunc(hModule);
                if (FreeLibrary(hModule)) {
                    logger.info("bot.plugin.unloaded", {name});
                    mPluginsMap1.erase(name);
                    mPluginsMap2.erase(hModule);
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
    for (auto& [name, hModule] : mPluginsMap1) {
        result.push_back(name);
    }
    return result;
}

bool PluginManager::hasPlugin(std::string const& name) { return mPluginsMap1.contains(name); }

HMODULE PluginManager::getPluginHandle(std::string const& name) { return mPluginsMap1[name]; }