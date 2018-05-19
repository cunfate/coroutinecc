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
    ptrdiff_t add(CoroutineFunc func, void* ud);
    void resume(int id);
    inline Coroutine::status status(int id) const;
    int running();
    void yield();
    inline shared_ptr<Coroutine>& operator[](size_t);

private:
    static const int STACK_SIZE = (1024*1024);
    static const int DEFAULT_COROUTINE = 16;
    static void mainfunc(uint32_t low32, uint32_t hi32);

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
    static enum status{
        COROUTINE_DEAD,
        COROUTINE_READY,
        COROUTINE_RUNNING,
        COROUTINE_SUSPEND
    };
    Coroutine(Scheduler* s, CoroutineFunc func, void* ud);
    ~Coroutine();
    status getStatus() const { return status_; }
    void setStatus(status stat) { status_ = stat; }
    ucontext_t& getContext() { return ctx_; }
    void saveStack(char *top);
    void inline runFunc(Scheduler* s);
}
}

#endif //__COROUTINE_CC_H