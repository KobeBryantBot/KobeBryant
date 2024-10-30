#include "ServiceManager.hpp"

ServiceManager& ServiceManager::getInstance() {
    static std::unique_ptr<ServiceManager> instance;
    if (!instance) {
        instance = std::make_unique<ServiceManager>();
    }
    return *instance;
}

bool ServiceManager::exportFuncPtr(std::string const& funcName, HMODULE hModule, FARPROC func) {
    auto key = ServiceFuncKey(hModule, funcName);
    if (!mExportedFunctions.contains(key)) {
        mExportedFunctions[key] = func;
        mPluginFunctions[hModule].insert(key);
        return true;
    }
    return false;
}

FARPROC ServiceManager::importFuncPtr(std::string const& pluginName, std::string const& funcName) {
    auto handle = PluginManager::getInstance().getPluginHandle(pluginName);
    auto key    = ServiceFuncKey(handle, funcName);
    if (mExportedFunctions.contains(key)) {
        return mExportedFunctions[key];
    }
    return nullptr;
}

bool ServiceManager::hasFunc(std::string const& pluginName, std::string const& funcName) {
    auto handle = PluginManager::getInstance().getPluginHandle(pluginName);
    auto key    = ServiceFuncKey(handle, funcName);
    return mExportedFunctions.contains(key);
}

void ServiceManager::removePluginFunc(HMODULE hModule) {
    for (auto& key : mPluginFunctions[hModule]) {
        mExportedFunctions.erase(key);
    }
    mPluginFunctions.erase(hModule);
}

void ServiceManager::removeAllFunc() {
    mPluginFunctions.clear();
    mExportedFunctions.clear();
}