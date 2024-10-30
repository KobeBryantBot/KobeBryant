#include "core/modules/PluginManager.hpp"
#include <unordered_map>
#include <unordered_set>

class ServiceManager {
private:
    std::unordered_map<std::string, FARPROC>                     mExportedFunctions;
    std::unordered_map<HMODULE, std::unordered_set<std::string>> mPluginFunctions;

public:
    static ServiceManager& getInstance();

    bool exportFuncPtr(std::string const& funcName, HMODULE hModule, FARPROC func);

    FARPROC importFuncPtr(std::string const& pluginName, std::string const& funcName);

    bool hasFunc(std::string const& pluginName, std::string const& funcName);

    void removePluginFunc(HMODULE hModule);

    void removeAllFunc();
};