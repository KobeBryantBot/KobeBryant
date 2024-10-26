#include "CommandManager.hpp"
#include "KobeBryant.hpp"
#include "PluginManager.hpp"
#include "api/CommandRegistry.hpp"
#include <conio.h>

CommandManager& CommandManager::getInstance() {
    static std::unique_ptr<CommandManager> instance;
    if (!instance) {
        instance = std::make_unique<CommandManager>();
    }
    return *instance;
}

void CommandManager::init() {
    std::string input;
    while (std::getline(std::cin, input)) {
        // 关闭程序命令
        if (input == "stop") {
            EXIST_FLAG = false;
            PluginManager::getInstance().unloadAllPlugins();
            KobeBryant::getInstance().getLogger().info("bot.main.stopping");
            KobeBryant::getInstance().getWsClient().Close();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            PRINT(tr("bot.main.exit"));
            _getch();
            return;
        } else if (input == "reload") {
            PluginManager::getInstance().unloadAllPlugins();
            PluginManager::getInstance().loadAllPlugins();
        } else if (input == "version") {
            KobeBryant::getInstance().getLogger().info(
                "bot.command.version",
                {BOT_NAME, S(KOBE_VERSION_MAJOR), S(KOBE_VERSION_MINOR), S(KOBE_VERSION_PATCH)}
            );
        } else {
            handleConsoleInput(input);
        }
    }
}

void CommandManager::handleConsoleInput(std::string const& input) {
    std::vector<std::string> params;
    std::istringstream       iss(input);
    std::string              temp;
    while (iss >> temp) {
        params.push_back(temp);
    }
    if (!params.empty()) {
        auto cmd = params[0];
        if (mCallbacks.contains(cmd) || mProtectedCommands.contains(cmd)) {
            return handleCommand(cmd, params, input);
        }
        return KobeBryant::getInstance().getLogger().error("command.unknown", {cmd});
    }
    KobeBryant::getInstance().getLogger().error("command.unknown", {""});
}

bool CommandManager::registerCommand(
    HMODULE                                              hModule,
    std::string const&                                   cmd,
    std::function<void(std::vector<std::string> const&)> callback
) {
    if (!cmd.empty() && !mCallbacks.contains(cmd) && !mProtectedCommands.contains(cmd)) {
        mCallbacks[cmd] = std::move(callback);
        mPluginCommands[hModule].insert(cmd);
        return true;
    }
    return false;
}

void CommandManager::handleCommand(
    std::string const&              cmd,
    std::vector<std::string> const& params,
    std::string const&              raw
) {
    auto& logger = KobeBryant::getInstance().getLogger();
    if (cmd == "plugins") {
        try {
            auto& manager = PluginManager::getInstance();
            if (params.size() == 2) {
                if (params[1] == "list") {
                    auto        plugins = manager.getAllPlugins();
                    auto        count   = plugins.size();
                    std::string info;
                    for (auto& plugin : plugins) {
                        info += plugin;
                        info += ", ";
                    }
                    info.pop_back();
                    info.pop_back();
                    return logger.info("command.plugins.list", {S(count), info});
                }
            } else if (params.size() == 3) {
                auto plugin = params[2];
                if (params[1] == "load") {
                    if (!manager.hasPlugin(plugin)) {
                        if (manager.loadPlugin(plugin)) {
                            return logger.info("command.plugins.load.success", {plugin});
                        }
                        return logger.error("command.plugins.load.failed", {plugin});
                    }
                    return logger.error("command.plugins.load.loaded", {plugin});
                } else if (params[1] == "unload") {
                    if (manager.hasPlugin(plugin)) {
                        if (manager.unloadPlugin(plugin)) {
                            return logger.info("command.plugins.unload.success", {plugin});
                        }
                        return logger.error("command.plugins.unload.failed", {plugin});
                    }
                    return logger.error("command.plugins.notFound", {plugin});
                } else if (params[1] == "reload") {
                    if (manager.hasPlugin(plugin)) {
                        if (manager.unloadPlugin(plugin)) {
                            if (manager.loadPlugin(plugin)) {
                                return logger.info("command.plugins.reload.success", {plugin});
                            }
                        }
                        return logger.error("command.plugins.reload.failed", {plugin});
                    }
                    return logger.error("command.plugins.notFound", {plugin});
                }
            }
            return logger.error("command.wrongArguments", {">>" + raw + "<<"});
        } catch (...) {
            return logger.error("command.wrongArguments", {">>" + raw + "<<"});
        }
    } else {
        if (auto callback = mCallbacks[cmd]) {
            try {
                callback(params);
            } catch (const std::exception& e) {
                logger.error("command.exception", {e.what()});
            }
        }
    }
}

bool CommandManager::unregisterCommand(HMODULE hModule, std::string const& cmd) {
    if (mCallbacks.contains(cmd)) {
        mCallbacks.erase(cmd);
        mPluginCommands[hModule].erase(cmd);
        return true;
    }
    return false;
}

void CommandManager::unregisterPluginCommands(HMODULE hModule) {
    for (auto& cmd : mPluginCommands[hModule]) {
        unregisterCommand(hModule, cmd);
    }
    mPluginCommands.erase(hModule);
}

void CommandManager::unregisterAllCommands() {
    mCallbacks.clear();
    mPluginCommands.clear();
}

/////////////////  API  ////////////////
CommandRegistry& CommandRegistry::getInstance() {
    static std::unique_ptr<CommandRegistry> instance;
    if (!instance) {
        instance = std::make_unique<CommandRegistry>();
    }
    return *instance;
}

bool CommandRegistry::registerCommand(
    HMODULE                                              hModule,
    std::string const&                                   cmd,
    std::function<void(std::vector<std::string> const&)> callback
) {
    return CommandManager::getInstance().registerCommand(hModule, cmd, std::move(callback));
}

bool CommandRegistry::unregisterCommand(HMODULE hModule, std::string const& cmd) {
    return CommandManager::getInstance().unregisterCommand(hModule, cmd);
}

void CommandRegistry::executeCommand(std::string const& command) {
    CommandManager::getInstance().handleConsoleInput(command);
}