#include "ServiceManager.hpp"
#include "PluginManager.hpp"

ServiceManager& ServiceManager::getInstance() {
    static std::unique_ptr<ServiceManager> instance;
    if (!instance) {
        instance = std::make_unique<ServiceManager>();
    }
    return *instance;
}

bool ServiceManager::exportAnyFunc(
    std::string const&      pluginName,
    std::string const&      funcName,
    Service::AnyFunc const& func
) {
    if (!hasAnyFunc(pluginName, funcName)) {
        mPluginFunctions[pluginName][funcName] = std::move(func);
        return true;
    }
    return false;
}

Service::AnyFunc ServiceManager::importAnyFunc(std::string const& pluginName, std::string const& funcName) {
    return mPluginFunctions[pluginName][funcName];
}

bool ServiceManager::hasAnyFunc(std::string const& pluginName, std::string const& funcName) {
    if (mPluginFunctions.contains(pluginName)) {
        return mPluginFunctions[pluginName].contains(funcName);
    }
    return false;
}

bool ServiceManager::removeAnyFunc(std::string const& pluginName, std::string const& funcName) {
    if (hasAnyFunc(pluginName, funcName)) {
        mPluginFunctions[pluginName].erase(funcName);
        return true;
    }
    return false;
}

void ServiceManager::removePluginFunc(std::string const& pluginName) {
    if (mPluginFunctions.contains(pluginName)) {
        mPluginFunctions[pluginName].clear();
    }
}

void ServiceManager::removeAllFunc() { mPluginFunctions.clear(); }