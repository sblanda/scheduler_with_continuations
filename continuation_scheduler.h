#ifndef CONTINUATION_SCHEDULER_H
#define CONTINUATION_SCHEDULER_H

#include <functional>
#include "cs_base.h"

template <typename R>
class Future;

template <typename R>
class Promise;

template <typename R, typename R2>
class ContinuationScheduler : public CSBase {
    using func_type = std::function<R2(R)>;

public:
    ContinuationScheduler(const Promise<R>& orig_p, Future<R2> cont_f, func_type f);

    virtual void schedule_continuation();

private:
    Promise<R>& orig_promise;
    Future<R2> cont_future;
    func_type func;
};

#include "continuation_scheduler.ipp"

#endif