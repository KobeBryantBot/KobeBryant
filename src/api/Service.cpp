#include "api/Service.hpp"
#include "core/modules/ServiceManager.hpp"

bool Service::exportFuncPtr(std::string const& funcName, HMODULE hModule, FARPROC func) {
    return ServiceManager::getInstance().exportFuncPtr(funcName, hModule, func);
}

FARPROC Service::importFuncPtr(std::string const& pluginName, std::string const& funcName) {
    return ServiceManager::getInstance().importFuncPtr(pluginName, funcName);
}

bool Service::hasFunc(std::string const& pluginName, std::string const& funcName) {
    return ServiceManager::getInstance().hasFunc(pluginName, funcName);
}

bool Service::removeFuncPtr(HMODULE hModule, std::string const& funcName) {
    return ServiceManager::getInstance().removeFuncPtr(hModule, funcName);
}