#include "ServiceManager.hpp"
#include "PluginManager.hpp"

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
    auto handle = PluginManager::getInstance().getNativePluginEngine().getPluginHandle(pluginName);
    auto key    = ServiceFuncKey(handle, funcName);
    if (mExportedFunctions.contains(key)) {
        return mExportedFunctions[key];
    }
    return nullptr;
}

bool ServiceManager::hasFunc(std::string const& pluginName, std::string const& funcName) {
    auto handle = PluginManager::getInstance().getNativePluginEngine().getPluginHandle(pluginName);
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

bool ServiceManager::removeFuncPtr(HMODULE hModule, std::string const& funcName) {
    auto key = ServiceFuncKey(hModule, funcName);
    if (mExportedFunctions.contains(key)) {
        mExportedFunctions.erase(key);
        mPluginFunctions[hModule].erase(key);
        return true;
    }
    return false;
}

bool ServiceFuncKey::operator==(const ServiceFuncKey& rhs) const {
    return (mHandle == rhs.mHandle) && (mName == rhs.mName);
}

size_t std::hash<ServiceFuncKey>::operator()(const ServiceFuncKey& obj) const {
    std::size_t hash1  = std::hash<HMODULE>{}(obj.mHandle);
    std::size_t hash2  = std::hash<std::string>{}(obj.mName);
    hash1             ^= hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2);
    hash2             ^= hash1 + 0x9e3779b9 + (hash2 << 6) + (hash2 >> 2);
    return hash1 ^ hash2;
}