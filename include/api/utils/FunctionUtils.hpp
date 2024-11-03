#pragma once
#include "Macros.hpp"
#include <Windows.h>
#include <functional>
#include <stdexcept>

namespace utils {

template <typename RetType, typename... Args>
using FunctionPtr = RetType (*)(Args...);

KobeBryant_NDAPI FARPROC GetFunctionAddress(LPCSTR funcName);

KobeBryant_NDAPI FARPROC GetFunctionAddress(LPCWSTR dllName, LPCSTR funcName);

template <typename RetType, typename... Args>
inline FunctionPtr<RetType, Args...> GetFunctionPtr(LPCSTR funcName) {
    if (auto funcPtr = GetFunctionAddress(funcName)) {
        return reinterpret_cast<FunctionPtr<RetType, Args...>>(funcPtr);
    }
    return nullptr;
}

template <typename RetType, typename... Args>
inline FunctionPtr<RetType, Args...> GetFunctionPtr(LPCWSTR dllName, LPCSTR funcName) {
    if (auto funcPtr = GetFunctionAddress(dllName, funcName)) {
        return reinterpret_cast<FunctionPtr<RetType, Args...>>(funcPtr);
    }
    return nullptr;
}

template <typename RetType, typename... Args>
inline RetType CallFunction(LPCSTR funcName, Args... args) {
    if (auto func = GetFunctionPtr<RetType, Args...>(funcName); func) {
        return func(std::forward<Args>(args)...);
    }
    throw std::runtime_error("Failed to call function: " + funcName);
}

template <typename RetType, typename... Args>
inline RetType CallFunction(LPCWSTR dllName, LPCSTR funcName, Args... args) {
    if (auto func = GetFunctionPtr<RetType, Args...>(dllName, funcName); func) {
        return func(std::forward<Args>(args)...);
    }
    throw std::runtime_error("Failed to call function: " + funcName);
}

} // namespace utils