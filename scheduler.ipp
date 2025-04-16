#include "active_object.h"
#include "future.h"

template <typename R>
Future<R> Scheduler::schedule(const func_type<R>& f)
{
    auto fut = workers[current]->enqueue(f);
    current = (current + 1) % workers.size();
    return fut;
}

template <typename R>
void Scheduler::schedule(const func_type<R>& f, Future<R> fut)
{
    workers[current]->enqueue(f, fut);
    current = (current + 1) % workers.size();
}

template <typename R>
void Scheduler::work_until_completed(const Promise<R>* p)
{
    auto tid = std::this_thread::get_id();
    for (auto& w : workers) {
        if (tid == w->get_thread_id()) {
            w->work_until_completed(p);
            return;
        }
    }
}
