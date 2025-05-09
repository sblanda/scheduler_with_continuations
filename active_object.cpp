#include <queue>
#include <thread>
#include <functional>
#include <stdexcept>

#include "active_object.h"
#include "scheduler.h"
#include "future.h"
#include "qentry_base.h"

using std::function;
using std::unique_lock;
using std::mutex;
using std::queue;
using std::thread;
using std::unique_lock;

// default construct everything
ActiveObject::ActiveObject(Scheduler* s)
    : scheduler{s}
{}

// assume the thread has been killed already
ActiveObject::~ActiveObject() 
{}

void ActiveObject::start() 
{
    th = thread([this]() {
        this->worker();
    });
}

void ActiveObject::shut_down() 
{
    this->enqueue<void>([this]() { this->working = false;});
}

void ActiveObject::stop() 
{
    working = false;
    cv.notify_all();
}

void ActiveObject::join() 
{
    if (th.joinable())
        th.join();
}

Scheduler* ActiveObject::get_scheduler() const {
    return scheduler;
}

std::thread::id ActiveObject::get_thread_id() const 
{
    return th.get_id();
}

void ActiveObject::worker() 
{
    working = true;

    while (working) {
        QEBase* qeb;
        {
            unique_lock<mutex> lck {queue_mutex};
            while (working && work_queue.empty()) {
                cv.wait(lck);
            }

            if (!working)
                break;

            qeb = work_queue.front();
            work_queue.pop();
        }
        qeb->run_and_complete();
        delete qeb;
    }
}