#ifndef FUTURE_H
#define FUTURE_H

#include <mutex>
#include <condition_variable>
#include <memory>
#include <iostream>
#include <string>
#include <functional>
#include <type_traits>

#include "cs_base.h"

class ActiveObject;
class Scheduler;

enum FutureState {
    NEW,
    COMPLETING,
    COMPLETED
};
static int root_id = 0;

constexpr bool DEBUG = false;

class IllegalFutureState { };

template <typename R>
class Promise {
public:
    Promise(ActiveObject* a) 
        : id{root_id++}, value{}, ao{a}, mux{},
          cs{nullptr}, state{NEW}, cv{}
    {
        diag("constructor");
    }

    bool is_complete() const {
        return state == COMPLETED;
    }

    R get() const;
    void complete(const R& v);

    ActiveObject* get_ao() const
    {
        return ao;
    }

    template <typename R2>
    Future<R2> then(const std::function<R2(R)>& c);
   
private:
    void diag(const std::string& func, const std::string& msg = "") const
    {
        if (DEBUG) {
            std::cout << id << ": promise::" << func
                      << ", state = " << state << std::endl;
            if (msg != "") {
                std::cout << "\t-->" << msg << std::endl;
            }
        }
    }

    int id;
    R value;
    ActiveObject* ao;
    std::unique_ptr<CSBase> cs;
    std::atomic<int> state;
    mutable std::mutex mux;
    mutable std::condition_variable cv;
};

template<>
class Promise<void> {
public:
    Promise(ActiveObject* a)
        : id{root_id++}, ao{a}, cs{nullptr},
          state{NEW}, mux{}, cv{}
    {
        diag("constructor");
    }

    bool is_complete() const {
        return state == COMPLETED;
    }

    void get() const;
    void complete();

    template<typename R2>
    Future<R2> then(const std::function<R2()>& c);

    ActiveObject* get_ao() const
    {
        return ao;
    }

private:
    int id;
    ActiveObject* ao;
    std::unique_ptr<CSBase> cs;
    std::atomic<int> state;
    mutable std::mutex mux;
    mutable std::condition_variable cv;

    void diag(const std::string& func, const std::string& msg = "") const
    {
        if (DEBUG) {
            std::cout << id << ": promise::" << func
                      << ", state = " << state << std::endl;
            if (msg != "") {
                std::cout << "\t-->" << msg << std::endl;
            }
        }
    }
};

// R must be default-constructable and assignable.
template<typename R>
class Future {
public:
    Future(ActiveObject* a)
        : promise {std::make_shared<Promise<R>>(a)}
    {}

    bool is_complete() const {
        return promise->is_complete();
    }

    R get() {
        return promise->get();
    }

    template <typename R2>
    Future<R2> then(std::function<R2(R)> c)
    {
        return promise->then(c);
    }


    std::shared_ptr<Promise<R>> get_promise_ptr() {
        return promise;
    }

private:
    std::shared_ptr<Promise<R>> promise;
};

template<>
class Future<void> {
public:
    Future(ActiveObject* a)
        : promise {std::make_shared<Promise<void>>(a)}
    {}

    bool is_complete() const {
        return promise->is_complete();
    }

    void get() {
        promise->get();
        return;
    }

    template <typename R2>
    Future<R2> then(std::function<R2()> c)
    {
        return promise->then(c);
    }

    std::shared_ptr<Promise<void>> get_promise_ptr() {
        return promise;
    }

private:
    std::shared_ptr<Promise<void>> promise;
};

#include "future.ipp"

#endif

