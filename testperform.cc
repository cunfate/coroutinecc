#include <cstdio>
#include <time.h>
#include <iostream>

#include "coroutine.h"

using namespace Coroutinecc;
using namespace std;

struct args {
    int n;
};

static void
foo(Scheduler* s, void* ud) {
    for(int i = 0; i < 100000; i++) {
        s->yield();
    }
}

static void
foo2(Scheduler* s, void* ud) {
    for(int i = 0; i < 10; i++) {
        s->yield();
    }
}

static void
test(Scheduler* s) {
    args arg1 = { 0 };
    args arg2 = { 200 };

    ptrdiff_t co1 = s->add(foo, &arg1);
    ptrdiff_t co2 = s->add(foo, &arg2);
    printf("switch times test start!\n");
    clock_t start = clock();
    while((*s)[co1] && (*s)[co2]) {
            if((*s)[co1])
                s->resume(co1);
            if((*s)[co2])
                s->resume(co2);
    }
    clock_t end = clock();
    double swicth_times = 200000 / ((double)(end - start) / CLOCKS_PER_SEC);
    printf("switch times test end!\n");
    printf("It can switch %lf times per second\n", swicth_times);
}

int
main() {
    Scheduler* s = new Scheduler();
    test(s);

    return 0;
}