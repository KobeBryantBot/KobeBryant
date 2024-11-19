## Logger.hpp

## 概览
`Logger`类提供了日志记录功能，允许设置日志级别、输出日志到控制台或文件，并支持国际化。

## 成员函数详细说明

### 枚举类
```cpp
enum class LogLevel : int {
    Trace = 0,
    Fatal = 1,
    Error = 2,
    Warn  = 3,
    Info  = 4, // 默认
    Debug = 5,
}
```

### 静态成员函数
作用：添加语言文件到日志系统。
- 重载1
```cpp
static void appendLanguage(std::string const& local, i18n::LangFile const& lang);
```
- 重载2
```cpp
static void appendLanguage(std::string const& local, std::string const& lang);
```
- 参数
- local：std::string - 表示本地化标识。
- lang：i18n::LangFile对象 - 表示语言文件。
- lang：std::string - 表示语言文件路径。

### 构造函数
作用：构造一个新的`Logger`实例。
```cpp
explicit Logger(std::string const& title = utils::getCurrentPluginName());
```
- 参数
- title：std::string - 日志标题，默认当前插件名称

### 设置函数
作用：设置日志标题。
```cpp
void setTitle(std::string const& title);
```
- 参数
- title：std::string - 日志标题。<br>

作用：设置日志级别
```cpp
void setLevel(LogLevel level);
```
- 参数
- level：`LogLevel`枚举 - 设置日志级别。<br>

作用：设置日志输出文件
```cpp
bool setFile(std::filesystem::path const& path);
```
- 参数
- path：文件路径 - 设置日志输出文件
- 返回值
- bool - 表示设置是否成功。

### 输出日志
你可以使用以下方式，向控制台输出不同级别的日志
```cpp
auto logger = Logger();
logger.debug("这是一条日志");
logger.info("这是一条日志 {}", 114514);
logger.warn("这是一条日志");
logger.error("这是一条日志");
logger.fatal("这是一条日志");
```