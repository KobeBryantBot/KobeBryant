## Service.hpp

## 概览
`Service`类提供了服务函数的导出和导入功能，允许在不同插件之间共享函数。

## 成员函数详细说明

### 函数导出
作用：导出一个函数，使其可以在其他插件中被调用。

重载1
```cpp
template <typename Ret, typename... Args>
static bool exportFunc(std::string const& funcName, FuncPtr<Ret, Args...> func);
```
- 参数
- funcName：函数名称。
- func：函数指针。
- 返回值
- bool - 表示导出是否成功。<br>

重载2
```cpp
template <typename Ret, typename... Args>
static bool exportFunc(std::string const& funcName, std::function<Ret(Args...)> const& func);
```
- 参数
- funcName：函数名称。
- func：函数对象。
- 返回值 bool  
  表示导出是否成功。

### 函数导入
作用：从一个指定插件导入一个函数。
```cpp
template <typename Ret, typename... Args>
static std::function<Ret(Args...)> importFunc(std::string const& pluginName, std::string const& funcName);
```
- 参数
- pluginName：插件名称。
- funcName：函数名称。
- 返回值：std::function<Ret(Args...)>  
  导入的函数对象。

### 删除导出函数
作用：删除一个已经导出的函数。 
```cpp
static bool removeFunc(std::string const& funcName);
```
- 参数
- pluginName：插件名称。
- funcName：函数名称。
- 返回值：bool  
  表示是否删除成功。