#include "api/Service.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <unordered_map>
#include <unordered_set>

class ServiceManager {
private:
    using AnyFunc = std::function<std::any(std::vector<std::any> const&)>;
    std::unordered_map<std::string, std::unordered_map<std::string, AnyFunc>> mPluginFunctions;

public:
    static ServiceManager& getInstance();

    bool exportAnyFunc(std::string const& pluginName, std::string const& funcName, AnyFunc const& func);

    AnyFunc importAnyFunc(std::string const& pluginName, std::string const& funcName);

    bool hasAnyFunc(std::string const& pluginName, std::string const& funcName);

    bool removeAnyFunc(std::string const& pluginName, std::string const& funcName);

    void removePluginFunc(std::string const& pluginName);

    void removeAllFunc();
};