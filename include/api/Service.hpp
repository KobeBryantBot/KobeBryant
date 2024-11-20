#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <any>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <variant>
#include <vector>

class Service {
    using AnyFunc = std::function<std::any(const std::vector<std::any>&)>;
    template <typename Ret, typename... Args>
    using FuncPtr = Ret (*)(Args...);

public:
    using ScriptTypeBase = std::variant<std::monostate, int64_t, double, std::string, bool>;
    using ScriptType     = std::variant<
            std::monostate,
            int64_t,
            double,
            std::string,
            bool,
            std::vector<ScriptTypeBase>,
            std::unordered_map<std::string, ScriptTypeBase>>;

    template <typename Ret, typename... Args>
    static inline bool exportFunc(const std::string& funcName, FuncPtr<Ret, Args...> func) {
        std::function<Ret(Args...)> function = func;
        return exportFunc(funcName, function);
    }

    template <typename Ret, typename... Args>
    static inline bool exportFunc(const std::string& funcName, const std::function<Ret(Args...)>& func) {
        auto pluginName = utils::getCurrentPluginName();
        auto anyFunc    = [func](const std::vector<std::any>& args) -> std::any {
            if (sizeof...(Args) != args.size()) {
                throw std::runtime_error("Wrong arguments count");
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
    static inline std::function<Ret(Args...)> importFunc(const std::string& pluginName, const std::string& funcName) {
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
                return cast_type<Ret>(result, "result");
            };
        }
    }

    KobeBryant_NDAPI static bool hasFunc(const std::string& pluginName, const std::string& funcName);

    static inline bool removeFunc(const std::string& funcName) {
        auto pluginName = utils::getCurrentPluginName();
        return removeFunc(pluginName, funcName);
    }

protected:
    template <typename T>
    static inline T cast_type(const std::any& any, const std::string& info = "arguments") {
        try {
            return std::any_cast<T>(any);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Wrong type of " + info);
        }
    }
    template <typename... Args, std::size_t... Is>
    static inline std::tuple<Args...> make_args(const std::vector<std::any>& vec, std::index_sequence<Is...>) {
        return std::make_tuple(cast_type<Args>(vec[Is])...);
    }
    KobeBryant_NDAPI static bool removeFunc(const std::string&, const std::string&);
    KobeBryant_NDAPI static bool exportAnyFunc(const std::string&, const std::string&, const Service::AnyFunc&);
    KobeBryant_NDAPI static Service::AnyFunc importAnyFunc(const std::string&, const std::string&);
};