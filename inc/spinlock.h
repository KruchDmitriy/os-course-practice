#ifndef __LOCK_H__
#define __LOCK_H__

#define SPLINLOCK_LOCKED   1
#define SPLINLOCK_UNLOCKED 0

#include <stdatomic.h>

struct spinlock {
    atomic_int locked;
};

void lock(struct spinlock *lock);
void unlock(struct spinlock *lock);

#endif /*__LOCK_H__*/
