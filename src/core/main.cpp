#include "Global.hpp"
#include "api/utils/StringUtils.hpp"
#include "modules/CommandManager.hpp"
#include "modules/EventDispatcher.hpp"
#include "modules/KobeBryant.hpp"
#include "modules/PluginManager.hpp"
#include <conio.h>

std::atomic<bool> EXIST_FLAG(true);

void printLogo() {
    PRINT(R"(                                                                     )");
    PRINT(R"(       _  __     _             ____                         _        )");
    PRINT(R"(      | |/ /    | |           |  _ \                       | |       )");
    PRINT(R"(      | ' / ___ | |__   ___   | |_) |_ __ _   _  __ _ _ __ | |_      )");
    PRINT(R"(      |  < / _ \| '_ \ / _ \  |  _ <| '__| | | |/ _' | `_ \| __|     )");
    PRINT(R"(      | . \ (_) | |_) |  __/  | |_) | |  | |_| | (_| | | | | |_      )");
    PRINT(R"(      |_|\_\___/|_.__/ \___|  |____/|_|   \__, |\__,_|_| |_|\__|     )");
    PRINT(R"(                                           __/ |                     )");
    PRINT(R"(                                          |___/                      )");
    PRINT(R"(                                                                     )");
    PRINT(R"(      --------------------   耐  摔  的  王   -------------------     )");
    PRINT(R"(                                                                     )");
}

void fixConsoleOutput() {
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hConsole, &dwMode)) {
            SetConsoleMode(hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
}

int main() {
    // 修复控制台输出兼容性问题
    fixConsoleOutput();
    // 初始化机器人核心
    auto& bot = KobeBryant::getInstance();
    bot.getLogger().info("bot.main.loadConfig");
    SetConsoleTitle(utils::stringtoWstring(tr("bot.console.title")).c_str());
    // 创建互斥锁
    HANDLE hMutex = CreateMutex(NULL, FALSE, bot.getProcessMutex().c_str());
    // 检查是否已经有一个实例在运行
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        bot.getLogger().error("bot.error.exist");
        CloseHandle(hMutex);
        PRINT(tr("bot.main.exit"));
        _getch();
        return 1;
    }
    printLogo();
    bot.printVersion();
    bot.getLogger().info("bot.main.connecting");
    bot.init();
    bot.connect();
    // 初始化事件系统
    EventDispatcher::getInstance().init();
    // 初始化插件
    PluginManager::getInstance().loadPluginEngines();
    PluginManager::getInstance().loadAllPlugins();
    // 初始化命令系统
    CommandManager::getInstance().init();
    // 释放互斥锁
    CloseHandle(hMutex);
    return 0;
};