#include <kernel/thread.h>
#include <kernel/cpu/cpu.h>
#include <kernel/spinlock.h>
#include <stdio.h>

static volatile int shared_counter = 0;
static spinlock_t lock = SPIN_STATIC_UNLOCKED;

static void *thread_func(void *arg) {
    int iterations = 10;
    for (int i = 0; i < iterations; i++) {
        spin_lock(&lock);
        int local_counter = ++shared_counter;
        spin_unlock(&lock);

        printf("CPU %d, iteration %d, shared_counter: %d\n", cpu_get_id(), i, local_counter);
    }
    return NULL;
}

void start_thread_on_cpu(int cpu_id) {
    struct thread *t = thread_create(THREAD_FLAG_SUSPENDED, thread_func, NULL);
    if (!t) {
        printf("Failed to create thread on CPU %d\n", cpu_id);
        return;
    }

    cpu_bind(cpu_id, t);
    thread_launch(t);
}
