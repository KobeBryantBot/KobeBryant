# KobeBryant

[![Latest Tag](https://img.shields.io/github/v/tag/KobeBryantBot/KobeBryant?label=Latest%20Tag&style=for-the-badge)](https://github.com/KobeBryantBot/KobeBryant/releases) 
[![Downloads](https://img.shields.io/github/downloads/KobeBryantBot/KobeBryant/total?style=for-the-badge&color=%2300ff00)](https://github.com/KobeBryantBot/KobeBryant/releases)  
![C++](https://img.shields.io/badge/C++-23-blue?logo=C%2B%2B&logoColor=41a3ed&style=for-the-badge)   
[![Contributors](https://img.shields.io/github/contributors/KobeBryantBot/KobeBryant.svg?style=for-the-badge)](https://github.com/KobeBryantBot/KobeBryant/graphs/contributors)
[![Stars](https://img.shields.io/github/stars/KobeBryantBot/KobeBryant.svg?style=for-the-badge)](https://github.com/KobeBryantBot/KobeBryant/stargazers)
[![Forks](https://img.shields.io/github/forks/KobeBryantBot/KobeBryant.svg?style=for-the-badge)](https://github.com/KobeBryantBot/KobeBryant/network/members)
[![Issues](https://img.shields.io/github/issues/KobeBryantBot/KobeBryant.svg?style=for-the-badge)](https://github.com/KobeBryantBot/KobeBryant/issues)
[![Pull Requests](https://img.shields.io/github/issues-pr/KobeBryantBot/KobeBryant?style=for-the-badge)](https://github.com/KobeBryantBot/KobeBryant/pulls)    
[![License](https://img.shields.io/github/license/KobeBryantBot/KobeBryant.svg?style=for-the-badge)](LICENSE)
  
[![982714789](https://img.shields.io/badge/QQ交流群%20982714789-pink?style=for-the-badge&logo=tencent%20qq)](https://qm.qq.com/q/78bKZ18A9O)


一套开源的，基于C++编写的基于 `Onebot11` 的高性能、扩展性强、模块化的QQ机器人框架  
基于 `Onebot11` 协议（正向WS连接） 

### 扩展性强（可跨语言）

觉得 `C++` 太难？学习成本太高？本框架没有要求插件必须使用 `C++` 编写。

本框架开放插件引擎注册，除了可以加载 `C++` 的原生插件以外，你还可以编写任意语言的插件引擎，做到跨语言加载任意语言的插件。  

我们已经编写并提供了 [Python插件引擎](https://github.com/KobeBryantBot/KobeBryantScriptEngine-Python)，可加载Python插件。  
未来我们还计划提供 `Node.js` 引擎，支持Node.js插件。  

以上这只是插件引擎的示例，插件引擎只是一个模块，本质上就是一个原生插件，因此插件引擎并不需要官方编写实现。  
只要你愿意，你可以编写支持任意你喜欢的语言对应的插件引擎，来加载对应语言的插件。

框架内置了强大的 `事件系统（EventBus）` 和 `远程函数导出/导入（Service）` 系统，可提供跨语言的事件发布和函数调用。  
即使是不同语言编写的插件，依旧能够实现相互交互。例如C++插件可以和Python插件直接进行交互。  

### 模块化

本框架是模块化的，只有核心部分 `KobeBryant.exe` 是必需的，其余模块（例如其它插件引擎）都是可选的，你可以只用核心部分运行原生插件，也可以加载其它插件引擎运行各种类型的插件（例如Python插件）   

编写模块是开放的，插件加载高度开放，正如前面所讲的，你可以编写任意你想要的插件引擎一样，你可以编写任意你需要的模块。

### 高性能  

使用 `C++` 编写，真正实现无锁多线程运行，带来最好的、最真实的性能。  
相同条件下，运行速度是目前常用的一些Python机器人框架的近百倍，实现真正的高性能，即使非常大的数据量也能吞吐自如。  

我们注重真实性能优化，必要时使用线程池进行管理，真正实现并发处理，而非打着异步噱头。（事实上，例如常用的Python里面，`async`和`await`的异步是协程实现的，依旧是单线程，只能在IO密集型环境下提高性能，而如果大量时间在解析数据上，使用异步不仅不会带来性能的提升，还会带来性能严重的下降，因为处理依旧是单线程，而创建异步需要额外开销）

# 支持情况

## 目前支持的系统和架构

| 系统和架构     | 支持情况           | 是否计划支持        | 支持的版本          |
| ------------- | ------------------ | ------------------ | ------------------ |
| Whndows-x64   | :white_check_mark: | :white_check_mark: | >= v0.1.0          |
| Whndows-arm64 | :x:                | :white_check_mark: | -                  |
| Linux-x64     | :x:                | :white_check_mark: | -                  |
| Linux-arm64   | :x:                | :white_check_mark: | -                  |
| macOS-x64     | :x:                | :x:                | -                  |
| macOS-arm64   | :x:                | :x:                | -                  |

> 注意：  
> 我们没有计划支持32位系统，也不要开支持32位系统的Issue。  
> 暂时没有计划支持macOS，如需支持，可自行 Fork 并 PR 。

# 使用方法

## 启动KobeBryant
- 从 [GitHub Release](https://github.com/KobeBryantBot/KobeBryant/releases) 下载你对应系统的最新版，解压后，将 `KobeBryant.exe` 放入一个文件夹中（最好是空文件夹）。
- 双击 `KobeBryant.exe` 运行机器人，第一次运行会自动生成相应的文件。
- 打开生成的 `config/config.json` 配置你的机器人（具体配置项后续介绍）。
- 向 `plugins` 文件夹内放入需要运行的插件（插件格式后续介绍）。
- 重启机器人。

> 注意：  
> 你不能更改可执行文件的名字，可执行文件的名字只能是 `KobeBryant.exe`   
> 如果你无法运行 `KobeBryant.exe` 或者运行后闪退，请更新 [VC运行库](https://aka.ms/vs/17/release/vc_redist.x64.exe)

## 插件热修改
- 你可以在控制台上面使用命令 `plugins list` 列出当前加载的插件
- 你可以在控制台上面使用命令 `plugins load/unload/reload <PluginName>` 来进行热加载、热卸载和热重载插件。
> 注意：热加载不会有任何问题，但是热卸载和热重载能否成功，取决于插件开发者是否支持

## 关闭KobeBryant
- 你可以在控制台上面使用命令 `stop` 安全关闭 `KobeBryant` 

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

# 插件介绍

> KobeBryant本体只能加载原生插件  
> KobeBryant在读取到非原生插件（`manifest.json`里面的`"type"`字段不是`"native"`时），会寻找已注册的插件引擎，并交由对应的插件引擎加载    
> 插件引擎本质上也是一个原生插件，你可以编写插件引擎来加载其它插件。

## 插件结构

每一个插件应该是一个文件夹，里面包含了插件运行需要的内容，和一个插件清单 `manifest.json`  
KobeBryant在运行时会读取每一个插件（文件夹）里面的 `manifest.json`，如果 `manifest.json` 里面的内容是合法的，则会视该文件夹为插件，并加载插件。  
原生插件会由KobeBryant直接加载，非原生插件会交给对应的引擎进行加载（如果有的话）。
由于引擎本身也是原生插件，在不指定插件依赖的情况下，原生插件总是最先加载。  

#### 插件基本结构应该如下所示
此处以原生插件为例，其余插件也是类似结构，只是入口文件类型可能不一样

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

## 原生插件
正如名字所描述的一样，原生插件也就是KobeBryant所原生支持的插件类型，通常由 `C++` 编写   
本处提供由 `C++` 编写的原生插件的示例。

> 当然，原生插件的本质是能被KobeBryant直接加载的插件，因此你也可以使用 `C`、 `Go` 、 `Rust` 等语言编写，但是由于KobeBryant直接提供的是 `C++` 的接口，因此你需要自己处理API的转化问题。

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
    // 注册的监听器、命令、Service可以不显式清理，会自动销毁
    // 使用KobeBryant内置的Schedule创建的定时任务可以不显式清理，会自动销毁
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
- 如果你有能力帮助我们贡献代码，你也可以 Fork 本仓库，并 [提交 Pull Requests](https://github.com/KobeBryantBot/KobeBryant/compare)

## 贡献者

这个项目之所以存在，离不开本项目所有的贡献者。

![贡献者](https://contrib.rocks/image?repo=KobeBryantBot/KobeBryant)