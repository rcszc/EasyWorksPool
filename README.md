# EasyWorksPool
- 一个超级简单的线程池.
- 添加到项目引用hpp即可使用.
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
try {
...
}
catch(EWP::ERR::ewp_error err) {
    err.what();
}
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
### 以下是一个简单的 Demo
```cpp
// 随便写的一个Demo测试类.
class TestClass {
protected:

    const char* temp_str = {};
    std::string temp_result = {};

    std::string Result(const char* A, int count) {

        return std::string(A) + " " + std::to_string(count);
    }

public:
    TestClass(std::string param, int count) {

        temp_str = param.c_str();

        for (int i = 0; i < count; ++i)
            std::cout << i << " " << temp_str << std::endl;

        temp_result = Result(temp_str, count);
    }
    ~TestClass() {}

    // 需要有一个获取结果的函数.
    std::string GetResult() {
        return temp_result;
    }
};

int main() {

    // 创建线程池, 初始化为16个工作线程.
    EWP::ThreadPool TestPool(16);

    // 异常检测.
    try {

        // 向线程池添加任务 <类>(构造参数)
        auto futureObjectA = TestPool.TP_PushTask<TestClass>("Hello World!", 2);
        auto futureObjectB = TestPool.TP_PushTask<TestClass>("Hello World!", 2);

        // 异步获取任务结果.
        if (futureObjectA.valid())
            std::cout << futureObjectA.get()->GetResult() << std::endl;

        if (futureObjectB.valid())
            std::cout << futureObjectB.get()->GetResult() << std::endl;
    }
    catch (EWP::ERR::ewp_error err) {

        // 使用EWP自带异常处理获取异常.
        std::cout << "Error: " << err.what() << std::endl;
    }

    return 0;
}
```
编写测试: VS2022 MSVC C++17 Release x64
