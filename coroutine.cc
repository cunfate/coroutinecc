#include "coroutine.h"

namespace Coroutinecc{

Scheduler::Scheduler(): 
    coroutine_num_(0), cap_(DEFAULT_COROUTINE), running_(-1), co_(vector<shared_ptr<Coroutine>>(DEFAULT_COROUTINE)) { }

Scheduler& Scheduler::open() {
    return *this;
}

void Scheduler::close() {
    co_.erase(co_.begin(), co_.end());
    running_ = -1;
    coroutine_num_ = 0;
}

Scheduler::~Scheduler() {
    this->close();
}

}