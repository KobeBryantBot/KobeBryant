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
            if (sizeof...(Args) != args.size()) {
                throw std::runtime_error("Wrong number of arguments.");
            }
            auto args_tuple = make_args<Args...>(args, std::index_sequence_for<Args...>{});
            if constexpr (std::is_void<Ret>::value) {
                std::apply(func, args_tuple);
                return std::any();
            } else {
                return std::any(std::apply(func, args_tuple));
            }
        };
        return exportAnyFunc(pluginName, funcName, anyFunc);
    }

    template <typename Ret, typename... Args>
    static inline std::function<Ret(Args...)> importFunc(std::string const& pluginName, std::string const& funcName) {
        auto func = importAnyFunc(pluginName, funcName);
        if constexpr (std::is_void<Ret>::value) {
            return [func](Args... args) {
                std::vector<std::any> anyArgs = {std::any(args)...};
                func(anyArgs);
            };
        } else {
            return [func](Args... args) -> Ret {
                std::vector<std::any> anyArgs = {std::any(args)...};
                std::any              result  = func(anyArgs);
                return std::any_cast<Ret>(result);
            };
        }
    }

    KobeBryant_NDAPI static bool hasFunc(std::string const& pluginName, std::string const& funcName);

    static inline bool removeFunc(std::string const& funcName) {
        auto pluginName = utils::getCurrentPluginName();
        return removeFunc(pluginName, funcName);
    }

protected:
    template <typename... Args, std::size_t... Is>
    static inline std::tuple<Args...> make_args(std::vector<std::any> const& vec, std::index_sequence<Is...>) {
        return std::make_tuple(std::any_cast<Args>(vec[Is])...);
    }

    KobeBryant_NDAPI static bool exportAnyFunc(std::string const&, std::string const&, Service::AnyFunc const&);

    KobeBryant_NDAPI static Service::AnyFunc importAnyFunc(std::string const&, std::string const&);

    KobeBryant_NDAPI static bool removeFunc(std::string const&, std::string const&);
};