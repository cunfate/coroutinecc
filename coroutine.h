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

class Scheduler;
class Coroutine;

typedef std::function<void (Scheduler*, void*)> CoroutineFunc;
using std::vector;
using std::shared_ptr;

class Coroutine: public std::enable_shared_from_this<Coroutine> {
public:
    typedef enum status{
        COROUTINE_DEAD,
        COROUTINE_READY,
        COROUTINE_RUNNING,
        COROUTINE_SUSPEND
    } status;
private:
    CoroutineFunc func_;
    void* ud_;
    ucontext_t ctx_;
    Scheduler* sch_;
    ptrdiff_t cap_;
    ptrdiff_t size_;
    Coroutine::status status_;
    char* stack_;
public:
    Coroutine(Scheduler* s, CoroutineFunc func, void* ud);
    ~Coroutine();
    Coroutine::status getStatus() const { return this->status_; }
    void setStatus(Coroutine::status stat) { this->status_ = stat; }
    ucontext_t& getContext() { return ctx_; }
    void saveStack(char *top);
    void inline runFunc(Scheduler* s);
    inline ptrdiff_t size() { return size_; }
    inline char* stack() { return stack_; }
    inline bool dead() { return status_ == status::COROUTINE_DEAD; }
};

class Scheduler {
public:

    static const int STACK_SIZE = (1024*1024);
    static const int DEFAULT_COROUTINE = 16;

    Scheduler();
    virtual ~Scheduler();
    Scheduler& open();
    void close();
    ptrdiff_t add(CoroutineFunc func, void* ud);
    void resume(int id);
    inline Coroutine::status status(int id) const;
    int running();
    void yield();
    inline shared_ptr<Coroutine>& operator[](size_t key) { return co_[key]; };

private:
    static void mainfunc(uint32_t low32, uint32_t hi32);

    //私有变量
    char stack_[STACK_SIZE];
    ucontext_t main_;
    int coroutine_num_;
    int cap_;
    int running_;
    vector<shared_ptr<Coroutine>> co_;
};

}

#endif //__COROUTINE_CC_H