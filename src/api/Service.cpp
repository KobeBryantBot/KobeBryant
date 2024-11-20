#include "api/Service.hpp"
#include "core/modules/ServiceManager.hpp"

bool Service::exportAnyFunc(const std::string& pluginName, const std::string& funcName, const Service::AnyFunc& func) {
    return ServiceManager::getInstance().exportAnyFunc(pluginName, funcName, func);
}

Service::AnyFunc Service::importAnyFunc(const std::string& pluginName, const std::string& funcName) {
    return ServiceManager::getInstance().importAnyFunc(pluginName, funcName);
}

bool Service::hasFunc(const std::string& pluginName, const std::string& funcName) {
    return ServiceManager::getInstance().hasAnyFunc(pluginName, funcName);
}

bool Service::removeFunc(const std::string& pluginName, const std::string& funcName) {
    return ServiceManager::getInstance().removeAnyFunc(pluginName, funcName);
}