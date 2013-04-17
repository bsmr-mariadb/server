#include <stdio.h>
#include <toku_assert.h>
#include <stdlib.h>
#include <signal.h>


static __attribute__((__noreturn__)) void catch_abort (int sig __attribute__((__unused__))) {
    exit(1);
}

int main(void) {
    signal (SIGABRT, catch_abort);
    assert(0);
    return 0;
}