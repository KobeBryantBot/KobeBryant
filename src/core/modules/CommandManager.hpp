#include "core/Global.hpp"
#include <unordered_set>

class CommandManager {
protected:
    std::unordered_set<std::string> mProtectedCommands = {"stop", "reload", "plugins", "version"};
    std::unordered_map<std::string, std::function<void(const std::vector<std::string>&)>> mCallbacks;
    std::unordered_map<std::string, std::unordered_set<std::string>>                      mPluginCommands;

public:
    static CommandManager& getInstance();

    void init();

    void handleConsoleInput(const std::string& input);

    bool registerSimpleCommand(
        const std::string&                                   plugin,
        const std::string&                                   cmd,
        std::function<void(const std::vector<std::string>&)> callback
    );

    bool unregisterCommand(const std::string& plugin, const std::string& cmd);

    void unregisterPluginCommands(const std::string& plugin);

    void handleCommand(const std::string& cmd, const std::vector<std::string>& params, const std::string& raw);

    void unregisterAllCommands();
};