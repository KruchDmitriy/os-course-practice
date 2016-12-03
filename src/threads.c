#include <threads.h>
#include <alloc.h>
#include <string.h>
#include <io.h>

#define STACK_SIZE 1024

static struct threads_desc_table th_table;
static struct spinlock locking_var;

// just for tests
static uint64_t main_thread_stack_ptr;

void idle(void*);

void run_thread(pid_t thread) {
    th_table.threads[thread].state = RUNNING;
    switch_new_thread((void**)&main_thread_stack_ptr, th_table.threads[thread].stack_ptr);
}

static void init_thread_desc(struct thread_desc* desc, pid_t id) {
    desc->state = READY_TO_CREATE;
    desc->thread_id = id;
}

void init_desc_table() {
    struct thread_desc* idle_thread = &th_table.threads[0];

    th_table.working_threads = idle_thread;

    idle_thread->thread_id = 0;
    idle_thread->state = IDLE_THREAD;
    idle_thread->stack_begin = mem_alloc(STACK_SIZE);
    uint64_t* stack_ptr = idle_thread->stack_begin + STACK_SIZE;
    idle_thread->stack_ptr = stack_ptr - sizeof(idle_thread->context);

    idle_thread->next = idle_thread;
    idle_thread->prev = NULL;

    struct thread_context* idle_contxt = &idle_thread->context;
    idle_contxt->func = (uintptr_t)idle;
    memcpy(idle_thread->stack_ptr, idle_contxt, sizeof(idle_thread->context));

    size_t i;
    for (i = 1; i < MAX_THREADS_COUNT; ++i) {
        struct thread_desc* desc = &th_table.threads[i];
        init_thread_desc(desc, i);
        desc->next = idle_thread;
        desc->prev = NULL;
    }

    idle_thread->next = NULL;
    th_table.cur_thread = idle_thread;
}

static struct thread_desc* find_first_unused() {
    struct thread_desc* threads = th_table.threads;
    for (int i = 1; i < MAX_THREADS_COUNT; i++) {
        if (threads[i].state == READY_TO_CREATE) {
            return &threads[i];
        }
    }

    return NULL;
}

pid_t create_thread(void (*fptr)(void *), void *arg) {
    lock(&locking_var);

    struct thread_desc* new_thread = find_first_unused();

    if (new_thread == NULL) {
        printf("Can't create thread (there are no free descriptors)!");
        return -1;
    }

    new_thread->next = th_table.working_threads;
    new_thread->prev = NULL;

    th_table.working_threads = new_thread;

    new_thread->stack_begin = mem_alloc(STACK_SIZE * sizeof(uint64_t));
    new_thread->state = READY_TO_RUN;

    /* place registers, func and args on stack */
    struct thread_context* th_contxt = &new_thread->context;
    th_contxt->func = (uintptr_t)fptr;
    th_contxt->func_arg = (uintptr_t)arg;
    th_contxt->exit_thread = exit_thread;
    th_contxt->thread_desc = (uint64_t)new_thread;

    uint64_t* stack_ptr = new_thread->stack_begin + STACK_SIZE;

    uint64_t* ptr = stack_ptr - sizeof(new_thread->context);
    memcpy(ptr, &new_thread->context, sizeof(new_thread->context));

    new_thread->stack_ptr = ptr;

    printf("Created thread id %d\n", new_thread->thread_id);
    printf("next thread id is %d\n", new_thread->next->thread_id);

    unlock(&locking_var);

    return new_thread->thread_id;
}

void join_thread(pid_t thread) {
    struct thread_desc* joined_thread = &th_table.threads[thread];
    while (1) {
        lock(&joined_thread->locking_var);

        if (joined_thread->state == FINISHED) {
            unlock(&joined_thread->locking_var);
            break;
        }

        unlock(&joined_thread->locking_var);
    }
}

void exit_thread(struct thread_desc* descr) {
    lock(&locking_var);

    printf("Thread with id %d exited successfully.\n", descr->thread_id);

    struct thread_desc* prev_thread = descr->prev;
    struct thread_desc* next_thread = descr->next;

    if (prev_thread != NULL) {
        prev_thread->next = next_thread;
    }
    next_thread->prev = prev_thread;

    mem_free(descr->stack_begin);
    descr->state = FINISHED;


    printf("Switching to thread %d\n", next_thread->thread_id);

    /*
        just for test
    */
    if (next_thread->state == IDLE_THREAD) {
        switch_threads((void **)(descr->stack_ptr), (void*)main_thread_stack_ptr);
    }

    if (next_thread->state == READY_TO_RUN) {
        next_thread->state = RUNNING;
        unlock(&locking_var);
        switch_new_thread((void **)(descr->stack_ptr), next_thread->stack_ptr);
    } else {
        next_thread->state = RUNNING;
        unlock(&locking_var);
        switch_threads((void **)(descr->stack_ptr), next_thread->stack_ptr);
    }
}
