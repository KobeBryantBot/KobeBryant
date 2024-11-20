#include "CommandManager.hpp"
#include "KobeBryant.hpp"
#include "PluginManager.hpp"
#include "api/CommandRegistry.hpp"
#include "core/Version.hpp"
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
    while (EXIST_FLAG && std::getline(std::cin, input)) {
        handleConsoleInput(input);
    }
}

void CommandManager::handleConsoleInput(const std::string& input) {
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

bool CommandManager::registerSimpleCommand(
    const std::string&                                   plugin,
    const std::string&                                   cmd,
    std::function<void(const std::vector<std::string>&)> callback
) {
    if (!cmd.empty() && !mCallbacks.contains(cmd) && !mProtectedCommands.contains(cmd)) {
        mCallbacks[cmd] = std::move(callback);
        mPluginCommands[plugin].insert(cmd);
        return true;
    }
    return false;
}

void CommandManager::handleCommand(
    const std::string&              cmd,
    const std::vector<std::string>& params,
    const std::string&              raw
) {
    auto& logger = KobeBryant::getInstance().getLogger();
    if (cmd == "stop") {
        EXIST_FLAG = false;
        PluginManager::getInstance().unloadAllPlugins();
        KobeBryant::getInstance().getLogger().info("bot.main.stopping");
        KobeBryant::getInstance().getWsClient().Close();
        fmt::print("{}\n", tr("bot.main.exit"));
        _getch();
        return;
    } else if (cmd == "reload") {
        PluginManager::getInstance().unloadAllPlugins();
        PluginManager::getInstance().loadAllPlugins();
    } else if (cmd == "version") {
        KobeBryant::getInstance().getLogger().info("command.version.result", {KOBE_VERSION_COLOR_STRING});
    } else if (cmd == "plugins") {
        try {
            auto& manager = PluginManager::getInstance();
            if (params.size() == 2) {
                if (params[1] == "list") {
                    auto plugins = manager.getAllPlugins();
                    if (!plugins.empty()) {
                        auto        count = plugins.size();
                        std::string info;
                        for (auto& plugin : plugins) {
                            info += plugin;
                            info += ", ";
                        }
                        info.erase(info.size() - 2);
                        return logger.info("command.plugins.list", {S(count), info});
                    }
                    return logger.info("command.plugins.list.empty");
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
            return logger.error("command.wrongArguments", {raw});
        }
        CATCH
    } else {
        if (auto callback = mCallbacks[cmd]) {
            try {
                callback(params);
            } catch (const std::exception& e) {
                logger.error("command.exception", {e.what()});
            }
            CATCH_END
        }
    }
}

bool CommandManager::unregisterCommand(const std::string& plugin, const std::string& cmd) {
    if (mPluginCommands[plugin].contains(cmd)) {
        mCallbacks.erase(cmd);
        mPluginCommands[plugin].erase(cmd);
        return true;
    }
    return false;
}

void CommandManager::unregisterPluginCommands(const std::string& plugin) {
    for (auto& cmd : mPluginCommands[plugin]) {
        unregisterCommand(plugin, cmd);
    }
    mPluginCommands.erase(plugin);
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

bool CommandRegistry::registerSimpleCommand(
    const std::string&                                   plugin,
    const std::string&                                   cmd,
    std::function<void(const std::vector<std::string>&)> callback
) {
    return CommandManager::getInstance().registerSimpleCommand(plugin, cmd, std::move(callback));
}

bool CommandRegistry::unregisterCommand(const std::string& plugin, const std::string& cmd) {
    return CommandManager::getInstance().unregisterCommand(plugin, cmd);
}

void CommandRegistry::executeCommand(const std::string& command) {
    CommandManager::getInstance().handleConsoleInput(command);
}

CommandRegistry::CommandRegistry() {}