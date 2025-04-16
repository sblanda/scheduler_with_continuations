#include <thread>

#include "active_object.h"
#include "scheduler.h"
#include "continuation_scheduler.h"

template <typename R>
R Promise<R>::get() const
{
    diag("get");
    if (state < COMPLETED) {
        // wait for completion
        diag("get", "we must wait");
        Scheduler* s = ao->get_scheduler();
        if (s->is_worker(std::this_thread::get_id())) {
            diag("get", "a worker thread -- run queue");
            s->work_until_completed(this);
        }
        else {
            diag("get", "non-worker thread. Sleeping");
            std::unique_lock lck(mux);
            while (state < COMPLETED) {
                cv.wait(lck);
            }
        }
    }
    diag("get", "returning value");
    return value;
}


template <typename R>
void Promise<R>::complete(const R& v) {
    int needed = NEW;
    if (state.compare_exchange_strong(needed, COMPLETING)) {
        diag("complete", "completing value");
        this->value = v;
        needed = COMPLETING;
        if (state.compare_exchange_strong(needed, COMPLETED)) {
            diag("complete", "notifying waiters");
            if(cs != nullptr)
                cs->schedule_continuation();
            cv.notify_all();
        } else {
            throw new IllegalFutureState;
        }
    } else {
        throw new IllegalFutureState;
    }
}


template <typename R>
template <typename R2>
Future<R2> Promise<R>::then(const std::function<R2(R)>& c)
{
    Future<R2> f2(this->ao);
    cs = std::make_unique<ContinuationScheduler<R, R2>>(*this, f2, c);
    if (state == COMPLETED)
        cs->schedule_continuation();
    return f2;
}

template <typename R2>
Future<R2> Promise<void>::then(const std::function<R2()>& c)
{
    Future<R2> f2(this->ao);
    cs = std::make_unique<ContinuationScheduler<void, R2>>(*this, f2, c);
    if (state == COMPLETED)
        cs->schedule_continuation();
    return f2;
}
