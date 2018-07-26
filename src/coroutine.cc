#include <cassert>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "coroutine.h"

namespace Coroutinecc{

Scheduler::Scheduler(): 
    coroutine_num_(0), cap_(DEFAULT_COROUTINE), running_(-1), co_(vector<shared_ptr<Coroutine>>(0)) { }

Scheduler& Scheduler::open() {
    return *this;
}

void 
Scheduler::close() {
    co_.erase(co_.begin(), co_.end());
    running_ = -1;
    coroutine_num_ = 0;
}

Scheduler::~Scheduler() {
    this->close();
}

/**
 * 向Schedular尾部插入Coroutine实例
 */
ptrdiff_t
Scheduler::add(CoroutineFunc func, void* ud) {
    Coroutine *co = new Coroutine(this, func, ud);
    this->co_.push_back(std::shared_ptr<Coroutine>(co));
    return this->co_.size() - 1;
}


/**
 * 恢复id指向的Coroutine实例
 */
void
Scheduler::resume(int id) {
    //std::assert(this->running_ == -1);
    //std::assert(id >= 0 && id < cap_);

    auto C = co_[id];
    uintptr_t ptr;
    if(!C) {
        return ;
    }

    Coroutine::status nowStatus = C->getStatus();
    ucontext_t& nowContext = C->getContext();
    switch(nowStatus) {
        case Coroutine::status::COROUTINE_READY:
            getcontext(&nowContext);                //初始化该协程的上下文
            nowContext.uc_stack.ss_sp = stack_;     //设置协程的uc_stack
            nowContext.uc_stack.ss_size = STACK_SIZE;
            nowContext.uc_link = &main_;            //设置该协程的后继上下文
            running_ = id;                          
            C->setStatus(Coroutine::status::COROUTINE_RUNNING);
            ptr = (uintptr_t)this;
            makecontext(&nowContext, (void(*)(void))mainfunc, 2, (uint32_t)ptr, (uint32_t)(ptr >> 32));    //为协程设置入口函数
            swapcontext(&main_, &nowContext);       //保存当前上下文到main_，激活nowContext指向的目标上下文
            break;
        case Coroutine::status::COROUTINE_SUSPEND:
            ::memcpy(stack_ + STACK_SIZE - C->size(), C->stack(), C->size());  
            running_ = id;
            swapcontext(&main_, &nowContext);
            break;
        default:
            assert(0);
    }
}

/**
 * 协程主动让出控制权，保存栈空间，恢复main_中保存的上下文（即恢复到Schduler::resume函数的上下文）
 */
void
Scheduler::yield() {
    assert(running_ >= 0);
    Coroutine& C = *co_[running_];     //获取正在运行的协程实例
    //assert((char*)(&C) > stack_);
    C.saveStack(stack_ + STACK_SIZE);   //保存当前栈空间
    C.setStatus(Coroutine::status::COROUTINE_SUSPEND);
    running_ = -1;
    swapcontext(&C.getContext(), &main_);
}

/**
 * 获取 id 指向协程实例的状态
 */
inline Coroutine::status
Scheduler::status(int id) const {
    assert(id >= 0 && id < cap_);
    if(!co_[id]) {
        return Coroutine::status::COROUTINE_DEAD;
    }

    return co_[id]->getStatus();
}

/**
 * 获取该调度器正在运行的协程实例ID
 */
int
Scheduler::running() {
    return running_;
}

/**
 * @param low32 调度表指针的低32位
 * @param hi32  调度表指针的高32位
 * 执行在Coroutine实例中注册的协程实体函数，函数执行完毕后清理此协程
 */
void
Scheduler::mainfunc(uint32_t low32, uint32_t hi32) {
    uintptr_t sptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
    Scheduler& S = *(Scheduler*)(sptr);
    int id = S.running();
    S[id]->runFunc(&S);
    S[id] = nullptr;
}

/**
 * @param s 管理协程实例的调度表
 * @param func 协程执行的实例
 * @param ud    协程运行时的参数
 * 初始化协程实例，初始运行状态为READY
 */
Coroutine::Coroutine(Scheduler* s, CoroutineFunc func, void* ud):
    func_(func),  ud_(ud), sch_(s), cap_(0), size_(0), status_(status::COROUTINE_READY), stack_(nullptr) {
        ::memset(&ctx_, 0, sizeof(ctx_));
}

/**
 * 协程实例的析构函数
 */
Coroutine::~Coroutine() {
    if(this->stack_)
        delete this->stack_;
}

/**
 * 保存当前栈空间到Coroutine实例的stack_空间中
 * @param top 当前栈的栈底
 * 由于栈空间是向低地址增长，因此栈底-当前函数栈的栈顶是需要保存的所有栈空间
 */
void    
Coroutine::saveStack(char* top) {
    char dummy = 0;
    assert(top - &dummy <= Scheduler::STACK_SIZE);
    if(cap_ < top - &dummy) {
        delete stack_;
        cap_ = top - &dummy;
        stack_ = new char[cap_];
    }
    size_ = top - &dummy;
    memcpy(stack_, &dummy, size_);
}

/**
 * @param s 管理协程的调度表实例
 * 运行协程实例
 */
void inline
Coroutine::runFunc(Scheduler* s) {
    func_(s, ud_);
}
}