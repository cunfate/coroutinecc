#include <assert>
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

ptrdiff_t
Scheduler::add(CoroutineFunc func, void* ud) {
    Coroutine *co = new Coroutine(this, func, ud);
    this->co_.push_back(make_shared(co));
    return this->co.size() - 1;
}

inline shared_ptr<Coroutine>& 
Scheduler::operator[](size_t key) {
    return co_[key];
}

void
Scheduler::resume(int id) {
    std::assert(this->running_ == -1);
    std::assert(id >= 0 && id < cap_);

    auto C = co_[id];
    if(!C) {
        return ;
    }

    Coroutine::status nowStatus = C->getStatus();
    ucontext_t& nowContext = C->getContext();
    switch(nowStatus) {
        case Coroutine::status::COROUTINE_READY:
            getcontext(&nowContext);
            nowContext.uc_stack.ss_sp = stack_;
            nowContext.uc_stack.ss_size = STACK_SIZE;
            nowContext.uc_link = main_;
            running_ = id;
            C->setStatus(Coroutine::status::COROUTINE_RUNNING);
            uintptr_t ptr = (uintptr_t)this;
            makecontext(&nowContext, (void(*)(void))mainfunc, (uint32_t)ptr, (uint32_t)(ptr >> 32));
            swapcontext(main_, &nowContext);
            break;
        case Coroutine::status::COROUTINE_SUSPEND:
            ::memcpy(stack_ + STACK_SIZE - C->size_, C->stack_, C->size_);
            running_ = id;
            swapcontext(main_, &nowContext);
            break;
        default:
            assert(0);
    }
}

void
Scheduler::yield() {
    assert(running_ >= 0);
    Coroutine& C = co_[id];
    assert(&C > stack_);
    C.saveStack(stack_ + STACK_SIZE);
    running_ = -1;
    swapcontext(&C.getContext(), &main_);
}

inline Coroutine::status
Scheduler::status(int id) {
    assert(id >= 0 && id < cap_);
    if(!co_[id]) {
        return Coroutine::status::COROUTINE_DEAD;
    }

    return co_[id]->status;
}

int
Scheduler::running() {
    return running_;
}

void
Scheduler::mainfunc(uint32_t low32, uint32_t hi32) {
    uintptr_t sptr = (uintptr_t)low32 | (uintptr_t)((uintptr_t)hi32 << 32);
    Scheduler* S = reinterpret_cast<Scheduler*>(sptr);
    int id = S->running();
    auto C = *S[id];
    C->runFunc(S);
    C = nullptr;
}


Coroutine::Coroutine(Scheduler* s, CoroutineFunc func, void* ud):
    func_(func),  ud_(ud), sch_(s), cap_(0), size_(0), status_(status::COROUTINE_READY), stack_(nullptr) { }

Coroutine::~Coroutine() {
    delete this->stack_;
}

void
Coroutine::saveStack(char* top) {
    char dummy = 0;
    assert(top - &dummy <= STACK_SIZE);
    if(cap_ < top - &dummy) {
        delete stack_;
        cap_ = top - &dummy;
        stack_ = new char[cap_];
    }
    size_ = top - &dummy;
    memcpy(stack_, &dummy, size_);
}

void inline
Coroutine::runFunc(Scheduler* s) {
    func_(s, ud_);
}
}