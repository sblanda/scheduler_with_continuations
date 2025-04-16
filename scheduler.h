#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <functional>
#include <thread>
#include <memory>
#include <vector>

template <typename R>
class Future;

template <typename R>
class Promise;

class ActiveObject;

class Scheduler {
    template <typename R>
    using func_type = std::function<R()>;

public:
    Scheduler(int num);

    template <typename R>
    Future<R> schedule(const func_type<R>& f);


    template <typename R>
    void schedule(const func_type<R>& f, Future<R> fut);

    template <typename R>
    void work_until_completed(const Promise<R>* p);
    
    bool is_worker(std::thread::id tid) const;
    
    void terminate();

private:
    std::vector<std::unique_ptr<ActiveObject>> workers;
    int current;
};

#include "scheduler.ipp"

#endif
