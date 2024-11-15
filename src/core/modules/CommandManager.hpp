#include "core/Global.hpp"
#include <unordered_set>

class CommandManager {
protected:
    std::unordered_set<std::string> mProtectedCommands = {"stop", "reload", "plugins", "version"};
    std::unordered_map<std::string, std::function<void(std::vector<std::string> const&)>> mCallbacks;
    std::unordered_map<std::string, std::unordered_set<std::string>>                      mPluginCommands;

public:
    static CommandManager& getInstance();

    void init();

    void handleConsoleInput(std::string const& input);

    bool registerSimpleCommand(
        std::string const&                                   plugin,
        std::string const&                                   cmd,
        std::function<void(std::vector<std::string> const&)> callback
    );

    bool unregisterCommand(std::string const& plugin, std::string const& cmd);

    void unregisterPluginCommands(std::string const& plugin);

    void handleCommand(std::string const& cmd, std::vector<std::string> const& params, std::string const& raw);

    void unregisterAllCommands();
};