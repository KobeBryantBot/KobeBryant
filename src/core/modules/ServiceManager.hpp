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

    bool exportAnyFunc(const std::string& pluginName, const std::string& funcName, AnyFunc const& func);

    AnyFunc importAnyFunc(const std::string& pluginName, const std::string& funcName);

    bool hasAnyFunc(const std::string& pluginName, const std::string& funcName);

    bool removeAnyFunc(const std::string& pluginName, const std::string& funcName);

    void removePluginFunc(const std::string& pluginName);

    void removeAllFunc();
};