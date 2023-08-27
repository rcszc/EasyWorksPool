// easy_workspool [EasyWorksPool]
// RCSZ 2023.08.28

#ifndef _EASY_WORKSPOOL_HPP
#define _EASY_WORKSPOOL_HPP

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <typeinfo>

// Easy Works[Thread] Pool.
namespace EWP {
    namespace SYSRTTI {

        struct tpc_object {
            std::string object_name; // ��������.
            std::size_t object_hash; // ����Hashֵ.
        };
        
        template<typename nClass>
        tpc_object TASKOBJECTINFO(nClass TaskObject) {
            tpc_object ResultInfo = {};

            // RunTime Type Identification [RTTI].
            const std::type_info& object_info = typeid(*TaskObject);

            ResultInfo.object_name = object_info.name();
            ResultInfo.object_hash = object_info.hash_code();

            return ResultInfo;
        }
    }

    namespace ERR {
        typedef const char* ERRINFO;

        // EWP �쳣����, ��д std::exception::what.
        class ewp_error :public std::exception {
        protected:
            std::string error_message = {};
        public:
            ewp_error(ERRINFO message) : error_message(message) {}

            ERRINFO what() const noexcept override {
                return error_message.c_str();
            }
        };
    }

    // EWP �̳߳�.
    // ������� uint32_t �����̳߳س�ʼ��С.
    class ThreadPool {
    protected:
        std::vector<std::thread>          thread_workers;
        std::queue<std::function<void()>> pool_tasks;  
        std::mutex                        queue_mutex;
        std::condition_variable           workers_condition;

        void thread_task_execute(uint32_t threads);
        void thread_task_free();

        bool thread_stop_flag = false;

    public:
        ThreadPool(uint32_t InitSetWorkers) {
            thread_task_execute(InitSetWorkers);
        };
        ~ThreadPool() {
            thread_task_free();
        };
        // Current creation objectinfo.
        SYSRTTI::tpc_object Tp_ObjectInfo = {};

        /*
        * ������������(��,����).
        * @param  Args_param... Args
        * @return std::future<std::shared_ptr<InClass>>
        */
        template<typename InClass, typename... Args_param>
        std::future<std::shared_ptr<InClass>> TP_PushTask(Args_param... Args) {
            auto push_task = std::make_shared<std::packaged_task<std::shared_ptr<InClass>()>>(
                [Args = std::make_tuple(std::forward<Args_param>(Args)...), this]() mutable {
                    try {
                        return std::apply([](auto&&... Args) {
                            return std::make_shared<InClass>(std::forward<decltype(Args)>(Args)...);
                        }, std::move(Args));
                    }
                    catch (...) {
                        throw ERR::ewp_error("Failed Create object");
                        return std::shared_ptr<InClass>(nullptr);
                    }
                });
            // create object => get object info.
            Tp_ObjectInfo = SYSRTTI::TASKOBJECTINFO(push_task);

            std::future<std::shared_ptr<InClass>> Result = push_task->get_future();
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                if (thread_stop_flag) {
                    // disable push task.
                    throw ERR::ewp_error("Failed Thread pool stop");
                    return Result;
                }
                else
                    pool_tasks.emplace([push_task]() { (*push_task)(); });
            }
            workers_condition.notify_one();

            return Result;
        }

        /*
        * ��ȡ�������Ŷӵ���������.
        * @param  void
        * @return uint32_t
        */
        uint32_t TP_TasksCount();

        /*
        * ���óش�С(Workers����).
        * @param  uint32_t
        * @return void
        */
        void TP_ResizeThreads(uint32_t ResizeThreads);
    };

    size_t getthis_thread_id_number();
}
// ��ȡthis�߳�ID���.
// @return size_t
#define THIS_THREAD_ID EWP::getthis_thread_id_number

#endif