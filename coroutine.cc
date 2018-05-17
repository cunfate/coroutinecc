#include "coroutine.h"

namespace Coroutinecc{

Scheduler::Scheduler(): 
    coroutine_num_(0), cap_(DEFAULT_COROUTINE), running_(-1){}

}