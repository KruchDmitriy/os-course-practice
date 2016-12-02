#include "spinlock.h"
#include "ints.h"

static int threads_pretend_cs;

void lock(struct spinlock *lock) {
    if (threads_pretend_cs == 0) {
        disable_ints();
    }

    do {
        while(atomic_load_explicit(&lock->locked,
                memory_order_relaxed) == SPLINLOCK_LOCKED);
    } while (atomic_exchange_explicit(&lock->locked,
                SPLINLOCK_LOCKED, memory_order_acquire) == SPLINLOCK_LOCKED);

    threads_pretend_cs++;
}

void unlock(struct spinlock *lock) {
    threads_pretend_cs--;

    atomic_store_explicit(&lock->locked, SPLINLOCK_UNLOCKED,
        memory_order_release);

    if (threads_pretend_cs == 0) {
        enable_ints();
    }
}
