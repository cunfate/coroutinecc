#ifndef __COROUTINE_CC_H
#define __COROUTINE_CC_H

#include <functional>
#include <vector>
#include <memory>
#include <cstddef>

#if __APPLE__ && __MACH__
	#include <sys/ucontext.h>
#else 
	#include <ucontext.h>
#endif 


namespace Coroutinecc{


typedef std::function<void (Scheduler*, void*)> CoroutineFunc;
using std::vector;
using std::shared_ptr;

class Scheduler {
public:
    Scheduler();
    virtual ~Scheduler();
    Scheduler& open();
    void close();
    void resume(int id);
    int status(int id);
    int running();
    void yield();

private:
    static const int STACK_SIZE = (1024*1024);
    static const int DEFAULT_COROUTINE = 16;

    //私有变量
    char stack_[STACK_SIZE];
    ucontext_t main_;
    int coroutine_num_;
    int cap_;
    int running_;
    vector<shared_ptr<Coroutine>> co_;
};

class Coroutine {
private:
    CoroutineFunc func_;
    void* ud_;
    ucontext_t ctx_;
    Scheduler* sch_;
    ptrdiff_t cap_;
    ptrdiff_t size_;
    int status_;
    char* stack_;
public:
    Coroutine(Scheduler* s, CoroutineFunc func, void* ud);
    ~Coroutine();
}
}

#endif //__COROUTINE_CC_H