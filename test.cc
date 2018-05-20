#include <cstdio>

#include "coroutine.h"

using namespace Coroutinecc;
using namespace std;

struct args {
    int n;
};

static void
foo(Scheduler* s, void* ud) {
    args* arg = reinterpret_cast<args*>(ud);
    int start = arg->n;
    for(int i = 0; i < 5; i++) {
        ::printf("coroutine %d : %d\n", s->running(), start + i);
        s->yield();
    }
}

static void
test(Scheduler* s) {
    args arg1 = { 0 };
    args arg2 = { 200 };

    ptrdiff_t co1 = s->add(foo, &arg1);
    ptrdiff_t co2 = s->add(foo, &arg2);
    printf("main start\n");
    while((*s)[co1] && (*s)[co2]) {
            if((*s)[co1])
                s->resume(co1);
            if((*s)[co2])
                s->resume(co2);
    }
    printf("main end\n");
}

int
main() {
    Scheduler* s = new Scheduler();
    test(s);

    return 0;
}