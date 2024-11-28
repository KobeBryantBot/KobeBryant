#pragma once
#include "Macros.hpp"
#include "api/utils/ModuleUtils.hpp"
#include <any>
#include <expected>
#include <format>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <variant>
#include <vector>

class Service {
    using AnyFunc = std::function<std::any(const std::vector<std::any>&)>;
    template <typename Ret, typename... Args>
    using FuncPtr = Ret (*)(Args...);
    template <typename T, typename E = std::string>
    using Expected = std::expected<T, E>;

public:
    /**
     * 导出一个函数
     * @param funcName 函数名
     * @param func 函数指针
     * @return 是否导出成功
     */
    template <typename Ret, typename... Args>
    static inline bool exportFunc(const std::string& funcName, FuncPtr<Ret, Args...> func) {
        return _exportFunc(funcName, (std::function<Ret(Args...)>)func);
    }

    /**
     * 导入一个函数
     * @param pluginName 插件名
     * @param funcName 函数名
     * @return 导入的函数
     */
    template <typename Ret, typename... Args>
    static inline Expected<std::function<Ret(Args...)>>
    importFunc(const std::string& pluginName, const std::string& funcName) {
        if (hasFunc(pluginName, funcName)) {
            auto func = importAnyFunc(pluginName, funcName);
            if constexpr (std::is_void<Ret>::value) {
                return [func](Args... args) {
                    std::vector<std::any> anyArgs = {to_any(args)...};
                    func(anyArgs);
                };
            } else {
                return [func](Args... args) -> Ret {
                    std::vector<std::any> anyArgs = {to_any(args)...};
                    std::any              result  = func(anyArgs);
                    return from_any<Ret>(result, "result");
                };
            }
        }
        return std::unexpected(std::format("Service {}::{} is not exported!", pluginName, funcName));
    }

    /**
     * 调用一个函数
     * @param pluginName 插件名
     * @param funcName 函数名
     * @param args 函数参数
     * @return 函数返回值
     */
    template <typename Ret, typename... Args>
    static inline Expected<Ret> callFunc(const std::string& pluginName, const std::string& funcName, Args... args) {
        if (auto func = importFunc<Ret, Args...>(pluginName, funcName)) {
            try {
                return func(std::forward<Args>(args)...);
            } catch (const std::exception& e) {
                return std::unexpected(
                    std::format("Fail to call Service: {}::{}\nException Caught: {}", pluginName, funcName, e.what())
                );
            } catch (...) {
                return std::unexpected(std::format("Fail to call Service: {}::{}", pluginName, funcName));
            }
        }
        return std::unexpected(std::format("Service {}::{} is not exported!", pluginName, funcName));
    }

    /**
     * 检查一个函数是否导出
     * @param pluginName 插件名
     * @param funcName 函数名
     * @return 是否导出
     */
    KobeBryant_NDAPI static bool hasFunc(const std::string& pluginName, const std::string& funcName);

    /**
     * 移除一个函数
     * @param pluginName 插件名
     * @param funcName 函数名
     * @return 是否移除成功
     */
    static inline bool removeFunc(const std::string& funcName) {
        auto pluginName = utils::getCurrentPluginName();
        return removeFunc(pluginName, funcName);
    }

protected:
    template <typename Ret, typename... Args>
    static inline bool _exportFunc(const std::string& funcName, const std::function<Ret(Args...)>& func) {
        auto pluginName = utils::getCurrentPluginName();
        auto anyFunc    = [func](const std::vector<std::any>& args) -> std::any {
            if (sizeof...(Args) != args.size()) {
                throw std::runtime_error("Wrong arguments count");
            }
            auto args_tuple = make_args<std::decay_t<Args>...>(args, std::index_sequence_for<std::decay_t<Args>...>{});
            if constexpr (std::is_void<Ret>::value) {
                std::apply(func, args_tuple);
                return std::any();
            } else {
                return to_any(std::apply(func, args_tuple));
            }
        };
        return exportAnyFunc(pluginName, funcName, anyFunc);
    }
    template <typename T>
    static inline T from_any(const std::any& any, const std::string& info = "arguments") {
        try {
            if constexpr (std::is_same_v<std::decay_t<T>, float> || std::is_same_v<std::decay_t<T>, double>) {
                return (T)std::any_cast<double>(any);
            } else if constexpr (std::is_convertible_v<std::decay_t<T>, int64_t>) {
                return (T)std::any_cast<int64_t>(any);
            } else return std::any_cast<T>(any);
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error(std::format("Wrong type of {}", info));
        }
    }
    template <typename T>
    static inline std::any to_any(T&& arg) {
        if constexpr (std::is_same_v<std::decay_t<T>, float> || std::is_same_v<std::decay_t<T>, double>) {
            return std::any((double)arg);
        } else if constexpr (std::is_convertible_v<std::decay_t<T>, int64_t>) {
            return std::any((int64_t)arg);
        } else {
            return std::any(arg);
        }
    }
    template <typename... Args, std::size_t... Is>
    static inline std::tuple<Args...> make_args(const std::vector<std::any>& vec, std::index_sequence<Is...>) {
        return std::make_tuple(from_any<Args>(vec[Is])...);
    }
    KobeBryant_NDAPI static bool removeFunc(const std::string&, const std::string&);
    KobeBryant_NDAPI static bool exportAnyFunc(const std::string&, const std::string&, const Service::AnyFunc&);
    KobeBryant_NDAPI static Service::AnyFunc importAnyFunc(const std::string&, const std::string&);
};