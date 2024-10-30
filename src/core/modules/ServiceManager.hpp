#include "core/modules/PluginManager.hpp"
#include <unordered_map>
#include <unordered_set>

struct ServiceFuncKey {
    HMODULE     mHandle;
    std::string mName;

    bool operator==(const ServiceFuncKey& rhs) const { return (mHandle == rhs.mHandle) && (mName == rhs.mName); }
};

struct ServiceFuncKey;
namespace std {
template <>
struct hash<ServiceFuncKey> {
    size_t operator()(const ServiceFuncKey& obj) const {
        std::size_t hash1  = std::hash<HMODULE>{}(obj.mHandle);
        std::size_t hash2  = std::hash<std::string>{}(obj.mName);
        hash1             ^= hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2);
        hash2             ^= hash1 + 0x9e3779b9 + (hash2 << 6) + (hash2 >> 2);
        return hash1 ^ hash2;
    }
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

    void removePluginFunc(HMODULE hModule);

    void removeAllFunc();
};