#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <stdexcept>

class Service {
public:
    template <typename Ret, typename... Args>
    using Func = Ret (*)(Args...);

protected:
    KobeBryant_NDAPI static bool    exportFuncPtr(std::string const& funcName, HMODULE hModule, FARPROC func);
    KobeBryant_NDAPI static FARPROC importFuncPtr(std::string const& pluginName, std::string const& funcName);
    KobeBryant_NDAPI static bool    removeFuncPtr(HMODULE hModule, std::string const& funcName);

public:
    KobeBryant_NDAPI static bool hasFunc(std::string const& pluginName, std::string const& funcName);

    template <class Fn>
    static bool exportFunc(std::string const& funcName, Fn&& func) {
        auto handle   = utils::getCurrentModuleHandle();
        auto funcProc = (FARPROC)func;
        return exportFuncPtr(funcName, handle, funcProc);
    }

    template <typename Ret, typename... Args>
    [[nodiscard]] static inline Func<Ret, Args...>
    importFunc(std::string const& pluginName, std::string const& funcName) {
        if (auto funcPtr = importFuncPtr(pluginName, funcName)) {
            return reinterpret_cast<Func<Ret, Args...>>(funcPtr);
        }
        return nullptr;
    }

    template <typename Ret, typename... Args>
    [[nodiscard]] static inline Ret callFunc(std::string const& pluginName, std::string const& funcName, Args... args) {
        if (auto funcPtr = importFunc<Ret, Args...>(funcName)) {
            return funcPtr(std::forward<Args>(args)...);
        }
        throw std::runtime_error("Failed to call func: " + funcName);
    }

    static inline bool removeFunc(std::string const& funcName) {
        auto handle = utils::getCurrentModuleHandle();
        return removeFuncPtr(handle, funcName);
    }
};