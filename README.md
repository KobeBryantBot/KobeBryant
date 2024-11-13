# KobeBryant

[![Latest Tag](https://img.shields.io/github/v/tag/KobeBryantBot/KobeBryant?label=最新版本&style=for-the-badge)](https://github.com/KobeBryantBot/KobeBryant/releases) 
![C++](https://img.shields.io/badge/C++-23-blue?logo=C%2B%2B&logoColor=41a3ed&style=for-the-badge)
  
[![982714789](https://img.shields.io/badge/QQ交流群%20982714789-pink?style=for-the-badge&logo=tencent%20qq)](https://qm.qq.com/q/78bKZ18A9O)


一套开源的，基于C++编写的基于 `Onebot11` 的高性能、扩展性强、模块化的QQ机器人框架  
基于 `Onebot11` 协议（正向WS连接） 

### 高性能  

使用 `C++` 编写，真正实现无锁多线程运行，带来最好的、最真实的性能。  
相同条件下，运行速度是目前常用的一些Python机器人框架的近百倍，实现真正的高性能，即使非常大的数据量也能吞吐自如。  

我们注重真实性能优化，必要时使用线程池进行管理，真正实现并发处理，而非打着异步噱头。（事实上，例如常用的Python里面，`async`和`await`的异步是协程实现的，依旧是单线程，只能在IO密集型环境下提高性能，而如果大量时间在解析数据上，使用异步不仅不会带来性能的提升，还会带来性能严重的下降，因为处理依旧是单线程，而创建异步需要额外开销）

### 模块化

本框架是模块化的，只有核心部分 `KobeBryant.exe` 是必需的，其余模块（例如其它插件引擎）都是可选的，你可以只用核心部分运行原生插件，也可以加载其它插件引擎运行各种类型的插件（例如Python插件）   

编写模块是开放的，你可以编写任意你需要的模块。

### 扩展性强（可跨语言）

觉得 `C++` 太难？学习成本太高？本框架没有要求插件必须使用 `C++` 编写。

本框架开放插件引擎注册，除了可以加载 `C++` 的原生插件意外，你还可以编写任意语言的插件引擎，做到跨语言加载任意语言的插件。  

我们已经编写并提供了 [Python插件引擎](https://github.com/KobeBryantBot/KobeBryantScriptEngine-Python)，可加载Python插件。  
这只是插件引擎的一个示例，只要你愿意，你可以开发任意任意语言的插件引擎。

框架内置了强大的 `事件系统(EventBus)` 和 `远程函数导出/导入(Service)` 系统，可提供跨语言的事件发布和函数调用。  
即使是不同语言编写的插件，依旧能够实现相互交互。例如C++插件可以和Python插件直接进行交互。

# 使用方法
- 从 [GitHub Release](https://github.com/KobeBryantBot/KobeBryant/releases) 下载最新版，放入一个文件夹中。
- 双击 `KobeBryant.exe` 运行机器人。
- 打开生成的 `config/config.json` 配置你的机器人（具体配置项后续介绍）。
- 向 `plugins` 文件夹内放入需要运行的插件。
- 重启机器人。

> 如果你无法运行 `KobeBryant.exe` 或者运行后闪退，请更新 [VC运行库](https://aka.ms/vs/17/release/vc_redist.x64.exe)

# 配置文件（config.json）
默认配置文件如下，请自行修改

```json
// config.json
{
    "ws_url": "ws://127.0.0.1:3001",         // WS连接的url（请使用Onebot11的正向WS连接）
    "token": "",                             // WS连接的Token
    "language": "zh_CN",                     // 插件语言（默认只附带了zh_CN.lang）
    "logger": {                              // 日志系统
        "colorLog": true,                    // 启用彩色输出（一般不需要禁用，除非你的系统不支持）
        "level": 4,                          // 输出等级（0为静默，1只输出严重错误，2输出错误以上等级信息，3输出警告以上信息，4输出正常信息，5输出中包含debug信息）
        "logFile": true,                     // 日志是否保存到文件
        "logFilePath": "./logs/latest.log"   // 日志文件路径
    }
}
```

# 构建项目
如果你愿意自己构建项目，或者向 `KobeBryant` 贡献代码，您可以按照以下说明自行构建项目

- 克隆仓库到本地，并使用你熟悉的 IDE （VS / VSCode / CLion等）打开项目目录，对 Cmake 项目进行编译生成。
- 正常构建该项目即可。  

# 原生插件（C++）介绍

> KobeBryant本体只能加载原生插件  
> 如果你需要加载其它类型的插件，请使用对应的插件引擎加载。  
> 插件引擎本质上也是一个原生插件，你可以编写插件引擎来加载其它插件。

## 插件结构

每一个插件应该是一个文件夹，里面包含了插件运行需要的内容，和一个插件清单 `manifest.json`  
插件结构应该如下所示

```text
├── ...
├── KobeBryant.exe
└── plugins
    └── PluginName（插件所在文件夹）
        ├── PluginEntry.dll（插件入口文件）
        ├── manifest.json
        └── ...（可选）
```

## 插件清单

KobeBryant在加载插件时，是先读取插件文件夹里面的 `manifest.json`   
你应该确保你的 `manifest.json` 正确填写

```json
// manifest.json
{
    "name": "PluginName",       // 必需：插件名，必须和文件夹名称一致
    "entry": "PluginEntry.dll", // 必需：插件入口文件
    "type": "native",           // 可选：（默认是"native"）原生插件请填写"native"，其它类型由插件引擎注册
    "preload": [],              // 可选：需要预加载的DLL
    "passive": false,           // 可选：如果插件不被其它插件依赖，是否跳过加载（一般纯前置插件才填写true）
    "dependence": [],           // 可选：插件依赖项（硬依赖）
    "optional_dependence": [],  // 可选：插件依赖项（可选依赖）
}
```
# 原生插件（C++）开发
> KobeBryant基于 `C++23` 编写，请确保你的开发环境和编译器支持 `C++23`  

## 开发流程
- 选择你最喜欢的IDE，例如 `Visual Studio`、`Clion`、`Visual Studio Code` 等等，并安装Cmake
- 安装 `Visual Studio` 并选择安装 `使用C++的桌面开发` 工具包
- 克隆我们提供的 [插件模板](https://github.com/KobeBryantBot/Native-Plugin-Template)，以及子模块 `SDK` 到你的开发目录下，并打开项目
- 开始编写你的插件
- 编译、测试、发布插件

## 代码示例
```C++
// Main.cpp
#include "api/EventBus.hpp"
#include "api/PacketSender.hpp"
#include "api/event/MessageEvent.hpp"

// 此处写插件加载时执行的操作
void onEnable() {
    // 运行插件
    EventBus::getInstance().subscribe<MessageEvent>([&](const MessageEvent& ev) {
        if (ev.mType == MessageType::Group && ev.mSubType == MessageSubType::Normal) {
            if (ev.mRawMessage.find("原神") != std::string::npos) {
                auto& sender = PacketSender::getInstance();
                sender.sendGroupMessage(
                    *ev.mGroup,
                    Message().at(ev.mSender).text(" 原神，启动！")
                );
            }
        }
    });
}

// 此处写插件卸载载时执行的操作
void onDisable() {
    // 卸载插件需要释放全部资源
    // 注册的监听器和命令可以不显式清理，会自动销毁
}
```

# 开源许可

版权所有 © 2024 KobeBryantBot, 保留所有权利.

本项目采用 LGPL-3.0 许可证发行 - 阅读 [LICENSE](LICENSE) 文件获取更多信息。   

- **开发者不对您负责，开发者没有义务为你编写代码、为你使用造成的任何后果负责**
1. 本框架全部代码采用 `LGPL 3.0` 开源协议。
2. 您可以使用任何开源许可证编写基于 `KobeBryant` 运行的插件或者引擎，甚至不发布您的源代码。
3. 如果你修改了 `KobeBryant` 的源代码，或者基于 `KobeBryant` 框架编写了一个新的框架，你必须开源它。
4. 如果你想要分发，转载本框架，你必须得到我们的授权！

## 参与贡献

欢迎参与贡献！  

- 你可以通过 [反馈问题](https://github.com/KobeBryantBot/KobeBryant/issues/new/choose) 帮助我们进行改进  
- 如果你有能力帮助我们贡献代码，你也可以 Fork 本仓库，并 [提交 Pull Requests](https://github.com/KobeBryantBot/KobeBryant-Cxx/compare)

## 贡献者

这个项目之所以存在，离不开本项目所有的贡献者。

![贡献者](https://contrib.rocks/image?repo=KobeBryantBot/KobeBryant)