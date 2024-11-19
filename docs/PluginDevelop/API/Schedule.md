## Schedule.hpp

## 概览
`Schedule`类提供了任务调度功能，允许延迟执行和重复执行任务。

## 成员函数详细说明

### 模板静态成员函数
作用：添加一个延迟执行的任务
```cpp
template <class T, class D>
static TaskID addDelayTask(std::chrono::duration<T, D> duration, Task const& task);
```
- 参数
- duration：std::chrono::duration<T, D> - 延迟时间。
- task：Task const&` - 要延迟执行的任务。<br>

作用：添加一个重复执行的任务，调用立即执行一次
```cpp
template <class T, class D>
static TaskID addRepeatTask(std::chrono::duration<T, D> duration, Task const& task, bool immediately = false);
```
- 参数 
- duration：std::chrono::duration<T, D> - 重复执行的时间隔。
- task：Task const& - 要重复执行的任务。
- immediately：bool - 是否立即执行一次，默认为false。
- 返回值
- TaskTD - 任务标识符<br>

作用：添加一个有限次数重复执行的任务，可以立即执行一次。
```cpp
template <class T, class D>
static TaskID addRepeatTask(std::chrono::duration<T, D> duration, Task const& task, bool immediately, uint64_t times);
```
- 参数
- duration：std::chrono::duration<T, D> - 重复执行的时间间隔。
- task：Task const& - 要重复执行的任务。
- immediately：bool - 是否立即执行一次。
- times：uint64_t - 重复执行的次数。
- 返回值
- TaskID - 任务标识符。<br>

作用：取消一个已经安排的任务。
```cpp
static bool cancelTask(TaskID id);
```
- 参数
- id：TaskID - 要取消的任务标识符。
- 返回值
- bool - 表示是否成功取消任务。
