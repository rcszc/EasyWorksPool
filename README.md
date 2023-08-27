# EasyWorksPool
- 一个超级简单的线程池.
- 添加到项目引用头即可使用.
- 可跨平台(应该)

### EWP 用法.
创建一个线程池, 并且初始化为32个工作线程.
```cpp
EWP::ThreadPool Demo(32);
```
向线程池提交一个任务类和构造参数.
```cpp
EWP::TP_PushTask<Test>(A, B, C);
```
异常监听(重写exception::what())
```cpp
EWP::ERR::ewp_error ERR;
```
获取线程池中正在排队的任务数量.
```cpp
uint32_t num = EWP::TP_TasksCount();
```
重新设置线程大小(此操作会释放所有任务然后重新分配大小)
```cpp
EWP::TP_ResizeThreads(128);
```
获取当前创建任务的对象信息(RTTI)
```cpp
struct tpc_object {
    std::string object_name; // 对象名称.
    std::size_t object_hash; // 对象Hash值.
};
EWP::SYSRTTI::tpc_object = EWP::Tp_ObjectInfo;
```
