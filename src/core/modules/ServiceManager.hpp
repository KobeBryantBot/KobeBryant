#include "api/Service.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <unordered_map>
#include <unordered_set>

class ServiceManager {
private:
    std::unordered_map<std::string, std::unordered_map<std::string, Service::AnyFunc>> mPluginFunctions;

public:
    static ServiceManager& getInstance();

    bool exportAnyFunc(std::string const& pluginName, std::string const& funcName, Service::AnyFunc const& func);

    Service::AnyFunc importAnyFunc(std::string const& pluginName, std::string const& funcName);

    bool hasAnyFunc(std::string const& pluginName, std::string const& funcName);

    bool removeAnyFunc(std::string const& pluginName, std::string const& funcName);

    void removePluginFunc(std::string const& pluginName);

    void removeAllFunc();
};