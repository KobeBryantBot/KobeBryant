#include "ServiceManager.hpp"
#include "PluginManager.hpp"

ServiceManager& ServiceManager::getInstance() {
    static std::unique_ptr<ServiceManager> instance;
    if (!instance) {
        instance = std::make_unique<ServiceManager>();
    }
    return *instance;
}

bool ServiceManager::exportAnyFunc(const std::string& pluginName, const std::string& funcName, AnyFunc const& func) {
    if (!hasAnyFunc(pluginName, funcName)) {
        mPluginFunctions[pluginName][funcName] = std::move(func);
        return true;
    }
    return false;
}

std::function<std::any(std::vector<std::any> const&)>
ServiceManager::importAnyFunc(const std::string& pluginName, const std::string& funcName) {
    return mPluginFunctions[pluginName][funcName];
}

bool ServiceManager::hasAnyFunc(const std::string& pluginName, const std::string& funcName) {
    if (mPluginFunctions.contains(pluginName)) {
        return mPluginFunctions[pluginName].contains(funcName);
    }
    return false;
}

bool ServiceManager::removeAnyFunc(const std::string& pluginName, const std::string& funcName) {
    if (hasAnyFunc(pluginName, funcName)) {
        mPluginFunctions[pluginName].erase(funcName);
        return true;
    }
    return false;
}

void ServiceManager::removePluginFunc(const std::string& pluginName) {
    if (mPluginFunctions.contains(pluginName)) {
        mPluginFunctions[pluginName].clear();
    }
}

void ServiceManager::removeAllFunc() { mPluginFunctions.clear(); }