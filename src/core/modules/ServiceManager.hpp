#include "core/modules/PluginManager.hpp"
#include <unordered_map>
#include <unordered_set>

struct ServiceFuncKey {
    HMODULE     mHandle;
    std::string mName;

    bool operator==(const ServiceFuncKey& rhs) const;
};

struct ServiceFuncKey;
namespace std {
template <>
struct hash<ServiceFuncKey> {
    size_t operator()(const ServiceFuncKey& obj) const;
};
} // namespace std

class ServiceManager {
private:
    std::unordered_map<ServiceFuncKey, FARPROC>                     mExportedFunctions;
    std::unordered_map<HMODULE, std::unordered_set<ServiceFuncKey>> mPluginFunctions;

public:
    static ServiceManager& getInstance();

    bool exportFuncPtr(std::string const& funcName, HMODULE hModule, FARPROC func);

    FARPROC importFuncPtr(std::string const& pluginName, std::string const& funcName);

    bool hasFunc(std::string const& pluginName, std::string const& funcName);

    bool removeFuncPtr(HMODULE hModule, std::string const& funcName);

    void removePluginFunc(HMODULE hModule);

    void removeAllFunc();
};