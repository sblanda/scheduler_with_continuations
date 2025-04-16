#include "future.h"
#include "queue_entry.h"

template <typename R>
Future<R> ActiveObject::enqueue(const func_type<R> func)
{
    std::unique_lock<std::mutex> lck { queue_mutex };
    auto fut = Future<R>(this);
    work_queue.push(new QueueEntry<R>(func, fut.get_promise_ptr()));
    cv.notify_one();
    return fut;
}

template<typename R>
void ActiveObject::enqueue(const func_type<R> func, Future<R> fut) {
    std::unique_lock<std::mutex> lck { queue_mutex };
    work_queue.push(new QueueEntry<R>(func, fut.get_promise_ptr()));
    cv.notify_one();
}


template <typename R>
void ActiveObject::work_until_completed(const Promise<R>* p)
{
    using namespace std::literals::chrono_literals;

    while (!p->is_complete()) {
        QEBase* qeb;
        {
            std::unique_lock lck {queue_mutex};
            while (working && work_queue.empty() && ! p->is_complete()) {
                cv.wait_for(lck, 3ms);
            }

            if (!working || p->is_complete())
                break;

            qeb = work_queue.front();
            work_queue.pop();
        }
        qeb->run_and_complete();
        delete qeb;
    }
}
