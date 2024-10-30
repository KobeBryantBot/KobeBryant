#include "ServiceManager.hpp"

ServiceManager& ServiceManager::getInstance() {
    static std::unique_ptr<ServiceManager> instance;
    if (!instance) {
        instance = std::make_unique<ServiceManager>();
    }
    return *instance;
}

bool ServiceManager::exportFuncPtr(std::string const& funcName, HMODULE hModule, FARPROC func) {
    auto pluginName = PluginManager::getInstance().getPluginName(hModule);
    auto keyName    = pluginName + "::" + funcName;
    if (!mExportedFunctions.contains(keyName)) {
        mExportedFunctions[keyName] = func;
        mPluginFunctions[hModule].insert(keyName);
        return true;
    }
    return false;
}

FARPROC ServiceManager::importFuncPtr(std::string const& pluginName, std::string const& funcName) {
    auto keyName = pluginName + "::" + funcName;
    if (mExportedFunctions.contains(keyName)) {
        return mExportedFunctions[keyName];
    }
    return nullptr;
}

bool ServiceManager::hasFunc(std::string const& pluginName, std::string const& funcName) {
    auto keyName = pluginName + "::" + funcName;
    return mExportedFunctions.contains(keyName);
}

void ServiceManager::removePluginFunc(HMODULE hModule) {
    for (auto& name : mPluginFunctions[hModule]) {
        mExportedFunctions.erase(name);
    }
    mPluginFunctions.erase(hModule);
}

void ServiceManager::removeAllFunc() {
    mPluginFunctions.clear();
    mExportedFunctions.clear();
}