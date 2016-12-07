#ifndef __THREADS_H__
#define __THREADS_H__

#include <list.h>
#include <spinlock.h>
#include <stdint.h>

#define MAX_THREADS_COUNT 256

typedef int pid_t;

struct thread_desc {
    uint64_t *stack_begin;
    uint64_t *stack_ptr;

    struct thread_context {
        uint64_t rflags;
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t rbx;
        uint64_t rbp;
        uintptr_t func;
        uintptr_t func_arg;
        void (*exit_thread)(struct thread_desc *);
        uintptr_t thread_desc;
    } __attribute__((packed))context;

    void *(*exec_func)(void *);
    void *args;

    enum {
        READY_TO_CREATE,
        READY_TO_RUN,
        RUNNING,
        WAITING,
        FINISHED,
        IDLE_THREAD
    } state;

    struct thread_desc* next;
    struct thread_desc* prev;
    pid_t thread_id;

    struct spinlock locking_var;
};

struct threads_desc_table {
    struct thread_desc threads[MAX_THREADS_COUNT];
    struct thread_desc* working_threads;
    struct thread_desc* cur_thread;
};

void switch_threads(void **prev , void* next);
void switch_new_thread(void **prev, void* next);
void run_thread(pid_t thread);

void init_desc_table();

pid_t create_thread(void (*fptr)(void *), void *arg);
void join_thread(pid_t thread);
void exit_thread();

#endif /*__THREADS_H__*/
