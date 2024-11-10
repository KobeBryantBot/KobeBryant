#include "api/Service.hpp"
#include "core/modules/ServiceManager.hpp"

bool Service::exportAnyFunc(std::string const& pluginName, std::string const& funcName, Service::AnyFunc const& func) {
    return ServiceManager::getInstance().exportAnyFunc(pluginName, funcName, func);
}

Service::AnyFunc Service::importAnyFunc(std::string const& pluginName, std::string const& funcName) {
    return ServiceManager::getInstance().importAnyFunc(pluginName, funcName);
}

bool Service::hasFunc(std::string const& pluginName, std::string const& funcName) {
    return ServiceManager::getInstance().hasAnyFunc(pluginName, funcName);
}

bool Service::removeFunc(std::string const& pluginName, std::string const& funcName) {
    return ServiceManager::getInstance().removeAnyFunc(pluginName, funcName);
}