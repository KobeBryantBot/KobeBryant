#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <any>
#include <functional>
#include <stdexcept>

class Service {
    using AnyFunc = std::function<std::any(std::vector<std::any> const&)>;
    template <typename Ret, typename... Args>
    using FuncPtr = Ret (*)(Args...);

public:
    template <typename Ret, typename... Args>
    static inline bool exportFunc(std::string const& funcName, FuncPtr<Ret, Args...> func) {
        std::function<Ret(Args...)> function = func;
        return exportFunc(funcName, function);
    }

    template <typename Ret, typename... Args>
    static inline bool exportFunc(std::string const& funcName, std::function<Ret(Args...)> const& func) {
        auto pluginName = utils::getCurrentPluginName();
        auto anyFunc    = [func](std::vector<std::any> const& args) -> std::any {
            auto args_tuple = any_vector_to_tuple<Args...>(args);
            auto result     = func(std::get<Args>(args_tuple)...);
            return result;
        };
        return exportAnyFunc(pluginName, funcName, anyFunc);
    }

    template <typename Ret, typename... Args>
    static inline std::function<Ret(Args...)> importFunc(std::string const& pluginName, std::string const& funcName) {
        auto func = importAnyFunc(pluginName, funcName);
        return [func](Args... args) -> Ret {
            std::vector<std::any> anyArgs = {std::any(args)...};
            std::any              result  = func(anyArgs);
            return std::any_cast<Ret>(result);
        };
    }

    KobeBryant_NDAPI static bool hasFunc(std::string const& pluginName, std::string const& funcName);

    static inline bool removeFunc(std::string const& funcName) {
        auto pluginName = utils::getCurrentPluginName();
        return removeFunc(pluginName, funcName);
    }

protected:
    template <typename... Args, std::size_t... Is>
    static inline std::tuple<Args...>
    any_vector_to_tuple_impl(std::vector<std::any> const& vec, std::index_sequence<Is...>) {
        return std::make_tuple(std::any_cast<Args>(vec[Is])...);
    }

    template <typename... Args>
    static inline std::tuple<Args...> any_vector_to_tuple(std::vector<std::any> const& vec) {
        if (sizeof...(Args) != vec.size()) {
            throw std::runtime_error("Vector size does not match the number of arguments.");
        }
        return any_vector_to_tuple_impl<Args...>(vec, std::index_sequence_for<Args...>{});
    }

    KobeBryant_NDAPI static bool
    exportAnyFunc(std::string const& pluginName, std::string const& funcName, Service::AnyFunc const& func);

    KobeBryant_NDAPI static Service::AnyFunc importAnyFunc(std::string const& pluginName, std::string const& funcName);

    KobeBryant_NDAPI static bool removeFunc(std::string const& pluginName, std::string const& funcName);
};