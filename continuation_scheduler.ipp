#include "scheduler.h"
#include "future.h"

template <typename R, typename R2>
ContinuationScheduler<R, R2>::ContinuationScheduler(const Promise<R>& orig_p,
                            Future<R2> cont_f,
                            func_type f)
    : orig_promise{orig_p}, cont_future{cont_f}, func{f}
{}

template <typename R, typename R2>
void ContinuationScheduler<R, R2>::schedule_continuation()
{
    Scheduler* s = orig_promise->get_ao()->get_scheduler();
    // orig_future should be completed
    R value = orig_promise->get();
    s->schedule<R2>([f = func, value]() -> R2 {
        return f(value);
    }, cont_future);
}

