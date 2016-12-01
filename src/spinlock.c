#include "spinlock.h"
#include "ints.h"

#define LOCKED   1
#define UNLOCKED 0

static int threads_pretend_cs;

void lock(struct spinlock *lock) {
    threads_pretend_cs++;

    if (threads_pretend_cs == 1) {
        disable_ints();
    }

    do {
        while(atomic_load_explicit(&lock->locked,
                memory_order_relaxed) == LOCKED);
    } while (atomic_exchange_explicit(&lock->locked,
                LOCKED, memory_order_acquire) == LOCKED);
}

void unlock(struct spinlock *lock) {
    atomic_store_explicit(&lock->locked, UNLOCKED,
        memory_order_release);

    threads_pretend_cs--;

    if (threads_pretend_cs == 0) {
        enable_ints();
    }
}
