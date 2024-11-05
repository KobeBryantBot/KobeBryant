#include "NativePluginEngine.hpp"
#include "CommandManager.hpp"
#include "EventBusImpl.hpp"
#include "KobeBryant.hpp"
#include "ScheduleManager.hpp"
#include "ServiceManager.hpp"
#include "api/utils/StringUtils.hpp"
#include "core/Global.hpp"

namespace fs = std::filesystem;

std::string NativePluginEngine::getPluginType() const { return "native"; }

std::string getErrorReason(unsigned long errorCode) {
    if (errorCode == 126) {
        return tr("bot.errorCode.126");
    } else if (errorCode == 127) {
        return tr("bot.errorCode.127");
    }
    return {};
}

bool NativePluginEngine::loadPlugin(std::string const& pluginName) {
    try {
        auto& logger = KobeBryant::getInstance().getLogger();
        if (fs::exists("./plugins/" + pluginName + "/manifest.json")) {
            if (auto manifest = PluginManifest::readFrom("./plugins/" + pluginName + "/manifest.json")) {
                if (manifest->mType == getPluginType()) {
                    auto name  = manifest->mName;
                    auto entry = utils::stringtoWstring("./plugins/" + pluginName + "/" + manifest->mEntry);
                    if (HMODULE hMoudle = LoadLibrary(entry.c_str())) {
                        mPluginsMap1[name]    = hMoudle;
                        mPluginsMap2[hMoudle] = name;
                        logger.info("bot.nativePlugin.loaded", {name});
                        return true;
                    } else {
                        DWORD errorCode = GetLastError();
                        auto  reason    = getErrorReason(errorCode);
                        logger.error("bot.nativePlugin.load.fail", {name, S(errorCode), reason});
                    }
                }
            }
        }
    }
    CATCH
    return false;
}

bool NativePluginEngine::unloadPlugin(std::string const& name) {
    try {
        if (mPluginsMap1.contains(name)) {
            auto hModule = mPluginsMap1[name];
            return unloadPlugin(hModule);
        }
    }
    CATCH
    return false;
}

bool NativePluginEngine::unloadPlugin(HMODULE hModule) {
    try {
        if (mPluginsMap2.contains(hModule)) {
            auto name = mPluginsMap2[hModule];
            EventBusImpl::getInstance().removePluginListeners(hModule);
            CommandManager::getInstance().unregisterPluginCommands(hModule);
            ScheduleManager::getInstance().removePluginTasks(hModule);
            ServiceManager::getInstance().removePluginFunc(hModule);
            if (FreeLibrary(hModule)) {
                mPluginsMap1.erase(name);
                mPluginsMap2.erase(hModule);
                KobeBryant::getInstance().getLogger().info("bot.nativePlugin.unloaded", {name});
                return true;
            }
        }
    }
    CATCH
    return false;
}

HMODULE NativePluginEngine::getPluginHandle(std::string const& name) { return mPluginsMap1[name]; }